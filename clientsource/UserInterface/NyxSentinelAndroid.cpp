#include "NyxSentinelAndroid.h"
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <link.h>
#include <mutex>
#include <string>
#include <thread>
#include <vector>
#include <unistd.h>
#include <sys/resource.h>
#include <android/log.h>

namespace
{
    using Clock = std::chrono::steady_clock;
    unsigned long long NowMs()
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            Clock::now().time_since_epoch()).count();
    }

    struct Segment { const unsigned char* data; size_t size; };
    struct Image { uintptr_t address; std::vector<Segment> segments; };

    int CollectImage(dl_phdr_info* info, size_t, void* opaque)
    {
        Image& image = *static_cast<Image*>(opaque);
        bool ownsAddress = false;
        for (size_t i = 0; i < info->dlpi_phnum; ++i)
        {
            const auto& p = info->dlpi_phdr[i];
            const uintptr_t start = info->dlpi_addr + p.p_vaddr;
            if (p.p_type == PT_LOAD && image.address >= start &&
                image.address - start < p.p_memsz)
                ownsAddress = true;
        }
        if (!ownsAddress) return 0;
        for (size_t i = 0; i < info->dlpi_phnum; ++i)
        {
            const auto& p = info->dlpi_phdr[i];
            if (p.p_type == PT_LOAD && (p.p_flags & PF_X) &&
                (p.p_flags & PF_R) && !(p.p_flags & PF_W) && p.p_filesz)
                image.segments.push_back({reinterpret_cast<const unsigned char*>(
                    info->dlpi_addr + p.p_vaddr), static_cast<size_t>(p.p_filesz)});
        }
        return 1;
    }

    // An in-process change fingerprint, NOT cryptographic attestation or a
    // replacement for signed builds. Never claim it proves a trusted client.
    uint64_t Fingerprint(const std::vector<Segment>& segments)
    {
        uint64_t hash = UINT64_C(14695981039346656037);
        for (const auto& segment : segments)
            for (size_t i = 0; i < segment.size; ++i)
                hash = (hash ^ segment.data[i]) * UINT64_C(1099511628211);
        return hash;
    }

    bool NewSession(std::string& session)
    {
        unsigned char bytes[16];
        const int fd = open("/dev/urandom", O_RDONLY | O_CLOEXEC);
        if (fd < 0) return false;
        size_t offset = 0;
        while (offset < sizeof(bytes))
        {
            const ssize_t n = read(fd, bytes + offset, sizeof(bytes) - offset);
            if (n <= 0) { close(fd); return false; }
            offset += static_cast<size_t>(n);
        }
        close(fd);
        static const char hex[] = "0123456789abcdef";
        session.clear();
        for (unsigned char b : bytes)
        {
            session += hex[b >> 4];
            session += hex[b & 15];
        }
        return true;
    }

    class Monitor
    {
    public:
        Monitor() : epoch(0), failure(false), ready(false), stop(false), digest(0),
            online(false), sequence(0), sentEpoch(0), lastSent(0), lastPump(0),
            lastProgress(0), observedEpoch(0), closeAt(0)
        {
            worker = std::thread([this] { Run(); });
        }
        ~Monitor()
        {
            { std::lock_guard<std::mutex> lock(mutex); stop = true; }
            wake.notify_all();
            worker.join();
        }
        void Run()
        {
            setpriority(PRIO_PROCESS, 0, 10); // Only this Linux worker thread.
            Image image;
            image.address = reinterpret_cast<uintptr_t>(&NyxSentinelAndroid::Pump);
            dl_iterate_phdr(CollectImage, &image);
            if (image.segments.empty()) { failure.store(true); return; }
            digest = Fingerprint(image.segments);
            epoch.store(1);
            ready.store(true);
            for (;;)
            {
                std::unique_lock<std::mutex> lock(mutex);
                if (wake.wait_for(lock, std::chrono::seconds(2), [this] { return stop; }))
                    return;
                lock.unlock();
                if (Fingerprint(image.segments) != digest)
                {
                    failure.store(true);
                    return;
                }
                epoch.fetch_add(1);
            }
        }
        bool Pump(bool gameOnline, std::string& message)
        {
            message.clear();
            const auto now = NowMs();
            const auto currentEpoch = epoch.load();
            // Android may freeze all threads while backgrounded. Allow the
            // worker to resume, but do not manufacture progress/heartbeats.
            if (!lastPump || now - lastPump > 5000 || !gameOnline ||
                currentEpoch != observedEpoch)
                lastProgress = now;
            lastPump = now;
            observedEpoch = currentEpoch;
            if (!gameOnline)
            {
                online = false;
                closeAt = 0;
                return true;
            }
            if (!online)
            {
                if (!NewSession(session)) return false;
                sequence = 0;
                sentEpoch = 0;
                lastSent = 0;
                lastProgress = now;
                online = true;
            }
            if (closeAt) return now < closeAt;
            if (failure.load() || now - lastProgress > 10000)
            {
                message = "NYX1|" + session + (failure.load() ?
                    "|ANDROID_CODE_INTEGRITY|executable-segment-check-failed" :
                    "|WORKER_STALLED|android-integrity-worker-stopped");
                closeAt = now + 1500;
                __android_log_print(ANDROID_LOG_ERROR, "NyxAndroid", "Integrity worker failure");
                return true;
            }
            if (ready.load() && currentEpoch > sentEpoch &&
                (!lastSent || now - lastSent >= 12000))
            {
                char buffer[160];
                snprintf(buffer, sizeof(buffer), "NYXH1|%s|%u|%u|%016llx.%u",
                    session.c_str(), ++sequence, static_cast<unsigned>(getpid()),
                    static_cast<unsigned long long>(digest), currentEpoch);
                message = buffer;
                lastSent = now;
                sentEpoch = currentEpoch;
                __android_log_print(ANDROID_LOG_INFO, "NyxAndroid",
                    "Heartbeat queued sequence=%u worker_epoch=%u", sequence, currentEpoch);
            }
            return true;
        }
    private:
        std::atomic<unsigned> epoch;
        std::atomic<bool> failure, ready;
        std::mutex mutex;
        std::condition_variable wake;
        bool stop;
        std::thread worker;
        uint64_t digest;
        bool online;
        std::string session;
        unsigned sequence, sentEpoch;
        unsigned long long lastSent, lastPump, lastProgress;
        unsigned observedEpoch;
        unsigned long long closeAt;
    };
}

bool NyxSentinelAndroid::Pump(bool gameOnline, std::string& message)
{
    static Monitor monitor;
    return monitor.Pump(gameOnline, message);
}
