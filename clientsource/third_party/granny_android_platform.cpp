#include "granny_file_reader.h"
#include "granny_file_operations.h"
#include "granny_memory.h"
#include "granny_system_clock.h"
#include "granny_threads.h"

#include <cstdio>
#include <ctime>
#include <sched.h>

#include "granny_cpp_settings.h"

USING_GRANNY_NAMESPACE;

BEGIN_GRANNY_NAMESPACE;
struct android_file_reader
{
    file_reader Base;
    FILE *Handle;
};
END_GRANNY_NAMESPACE;

static CALLBACK_FN(void) AndroidCloseFileReader(file_reader *ReaderInit)
{
    android_file_reader *Reader = (android_file_reader *)ReaderInit;
    if (Reader)
    {
        if (Reader->Handle)
            fclose(Reader->Handle);
        Deallocate(Reader);
    }
}

static CALLBACK_FN(int32x) AndroidReadAtMost(file_reader *ReaderInit,
                                              int32x FilePosition,
                                              int32x UInt8Count,
                                              void *Buffer)
{
    android_file_reader *Reader = (android_file_reader *)ReaderInit;
    if (!Reader || !Reader->Handle || FilePosition < 0 || UInt8Count <= 0)
        return 0;
    if (fseek(Reader->Handle, (long)FilePosition, SEEK_SET) != 0)
        return 0;
    return (int32x)fread(Buffer, 1, (size_t)UInt8Count, Reader->Handle);
}

static CALLBACK_FN(bool) AndroidGetReaderSize(file_reader *ReaderInit, int32x *SizeVar)
{
    android_file_reader *Reader = (android_file_reader *)ReaderInit;
    if (!Reader || !Reader->Handle || !SizeVar)
        return false;
    long Original = ftell(Reader->Handle);
    if (Original < 0 || fseek(Reader->Handle, 0, SEEK_END) != 0)
        return false;
    long Size = ftell(Reader->Handle);
    fseek(Reader->Handle, Original, SEEK_SET);
    if (Size < 0 || Size > 0x7fffffffL)
        return false;
    *SizeVar = (int32x)Size;
    return true;
}

CALLBACK_FN(file_reader *) GRANNY CreatePlatformFileReaderInternal(char const *FileNameToOpen)
{
    FILE *Handle = fopen(FileNameToOpen, "rb");
    if (!Handle)
        return 0;
    android_file_reader *Reader = Allocate(android_file_reader, AllocationUnknown);
    if (!Reader)
    {
        fclose(Handle);
        return 0;
    }
    InitializeFileReader(AndroidCloseFileReader, AndroidReadAtMost,
                         AndroidGetReaderSize, Reader->Base);
    Reader->Handle = Handle;
    return &Reader->Base;
}

open_file_reader_callback *GRANNY OpenFileReaderCallback = CreatePlatformFileReaderInternal;

void GRANNY RequeryTimerFrequency() {}

void GRANNY GetSystemSeconds(system_clock *Result)
{
    if (!Result)
        return;
    timespec Value;
    clock_gettime(CLOCK_MONOTONIC, &Value);
    int64x Microseconds = (int64x)Value.tv_sec * 1000000 + Value.tv_nsec / 1000;
    *((int64x *)Result) = Microseconds;
}

real32 GRANNY GetSecondsElapsed(system_clock const &StartClock,
                                system_clock const &EndClock)
{
    int64x Start = *((int64x const *)&StartClock);
    int64x End = *((int64x const *)&EndClock);
    return End > Start ? (real32)((End - Start) / 1000000.0) : 0.0f;
}

void GRANNY ThreadYieldToAny()
{
    sched_yield();
}
