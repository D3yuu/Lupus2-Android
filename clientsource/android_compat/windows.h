#pragma once

#if !defined(__ANDROID__) && !defined(__IOS__)
  #error "This windows.h compatibility layer is only for mobile builds!"
#endif

// Minimum stub for Windows types required by the engine.

#include <stdint.h>
#include <stddef.h>
#include <pthread.h>
#include <semaphore.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <alloca.h>
#include <time.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <errno.h>
#include <ctype.h>
#include <stdio.h>
#include <wchar.h>
#define _chdir chdir

#ifdef __cplusplus
extern "C" {
#endif

#define WIN32_LEAN_AND_MEAN
#define LUPUS2_MOBILE_WIN_TYPES 1
// Keep the old name while the remaining Android-era include guards are
// migrated.  Both platforms consume exactly the same legacy Win32 types.
#define LUPUS2_ANDROID_WIN_TYPES 1
#ifndef MB_ICONSTOP
#define MB_ICONSTOP 0x00000010L
#endif
#ifndef IN
#define IN
#define OUT
#endif
#ifndef LOWORD
#define LOWORD(value) ((WORD)((uintptr_t)(value) & 0xffffu))
#define HIWORD(value) ((WORD)(((uintptr_t)(value) >> 16) & 0xffffu))
#define MAKELONG(low, high) ((LONG)(((WORD)(low)) | ((DWORD)((WORD)(high))) << 16))
#endif

#ifndef WM_CHAR
#define WM_CHAR 0x0102
#endif

#ifndef strcmpi
#define strcmpi strcasecmp
#endif

typedef int BOOL;
typedef BOOL* LPBOOL;
typedef uint8_t BOOLEAN;
typedef int32_t LONG;
typedef uint32_t DWORD;
typedef uint16_t WORD;
typedef uint8_t BYTE;
typedef void* HANDLE;
typedef void* LPVOID;
typedef const void* LPCVOID;
typedef uint32_t UINT;
typedef int32_t INT;
typedef float FLOAT;
typedef void* HWND;
typedef void* HDC;
typedef void* HGLRC;
typedef void* HMODULE;
typedef void* HINSTANCE;
typedef void* HBITMAP;
typedef void* HFONT;
typedef void* HGDIOBJ;
typedef void* HICON;
typedef long LRESULT;
typedef long HRESULT;
typedef unsigned long ULONG;
typedef size_t SIZE_T;
typedef uintptr_t WPARAM;
typedef intptr_t LPARAM;
typedef void VOID;
typedef char CHAR;
typedef char* PCHAR;
typedef void* PVOID;
typedef unsigned char* PUCHAR;
typedef char* LPSTR;
typedef const char* LPCSTR;
typedef BYTE* LPBYTE;
typedef wchar_t WCHAR;
typedef WCHAR* LPWSTR;
typedef const WCHAR* LPCWSTR;
typedef char TCHAR;
typedef LRESULT (*WNDPROC)(HWND, UINT, WPARAM, LPARAM);
typedef WORD LANGID;

#ifndef DEFAULT_CHARSET
#define DEFAULT_CHARSET 1
#define SHIFTJIS_CHARSET 128
#define HANGUL_CHARSET 129
#define GB2312_CHARSET 134
#define CHINESEBIG5_CHARSET 136
#define GREEK_CHARSET 161
#define TURKISH_CHARSET 162
#define HEBREW_CHARSET 177
#define ARABIC_CHARSET 178
#define BALTIC_CHARSET 186
#define RUSSIAN_CHARSET 204
#define THAI_CHARSET 222
#define EASTEUROPE_CHARSET 238
#define VIETNAMESE_CHARSET 163
#endif
typedef void* HIMC;
typedef void* HIMCC;
typedef void* HKL;
typedef int* PINT;
typedef UINT* PUINT;
typedef struct _INPUTCONTEXT { int unused; } INPUTCONTEXT;
typedef struct _OSVERSIONINFOA { DWORD dwOSVersionInfoSize; } OSVERSIONINFOA;
typedef struct _WIN32_FIND_DATA {
  DWORD dwFileAttributes;
  DWORD nFileSizeHigh;
  DWORD nFileSizeLow;
  char cFileName[260];
  char cAlternateFileName[14];
} WIN32_FIND_DATA;
typedef struct _SECURITY_ATTRIBUTES {
  DWORD nLength;
  LPVOID lpSecurityDescriptor;
  BOOL bInheritHandle;
} SECURITY_ATTRIBUTES, *LPSECURITY_ATTRIBUTES;
typedef struct timecaps_tag {
  UINT wPeriodMin;
  UINT wPeriodMax;
} TIMECAPS;
typedef struct _GUID {
  uint32_t Data1;
  uint16_t Data2;
  uint16_t Data3;
  uint8_t Data4_0, Data4_1, Data4_2, Data4_3;
  uint8_t Data4_4, Data4_5, Data4_6, Data4_7;
} GUID;

#ifndef LF_FACESIZE
#define LF_FACESIZE 32
#endif

typedef struct _LOGFONTA {
  LONG lfHeight;
  LONG lfWidth;
  LONG lfEscapement;
  LONG lfOrientation;
  LONG lfWeight;
  BYTE lfItalic;
  BYTE lfUnderline;
  BYTE lfStrikeOut;
  BYTE lfCharSet;
  BYTE lfOutPrecision;
  BYTE lfClipPrecision;
  BYTE lfQuality;
  BYTE lfPitchAndFamily;
  CHAR lfFaceName[LF_FACESIZE];
} LOGFONTA, LOGFONT;

typedef struct _TEXTMETRICA { LONG tmHeight; } TEXTMETRICA, TEXTMETRIC;

typedef struct _BITMAPINFOHEADER {
  DWORD biSize;
  LONG biWidth;
  LONG biHeight;
  WORD biPlanes;
  WORD biBitCount;
  DWORD biCompression;
  DWORD biSizeImage;
  LONG biXPelsPerMeter;
  LONG biYPelsPerMeter;
  DWORD biClrUsed;
  DWORD biClrImportant;
} BITMAPINFOHEADER;
typedef struct _RGBQUAD { BYTE rgbBlue, rgbGreen, rgbRed, rgbReserved; } RGBQUAD;
typedef struct _BITMAPINFO { BITMAPINFOHEADER bmiHeader; RGBQUAD bmiColors[1]; } BITMAPINFO;

typedef pthread_mutex_t CRITICAL_SECTION;
static inline void InitializeCriticalSection(CRITICAL_SECTION* section) { pthread_mutex_init(section, NULL); }
static inline void DeleteCriticalSection(CRITICAL_SECTION* section) { pthread_mutex_destroy(section); }
static inline void EnterCriticalSection(CRITICAL_SECTION* section) { pthread_mutex_lock(section); }
static inline void LeaveCriticalSection(CRITICAL_SECTION* section) { pthread_mutex_unlock(section); }
static inline BOOL TryEnterCriticalSection(CRITICAL_SECTION* section) { return pthread_mutex_trylock(section) == 0; }

#define TRUE 1
#define FALSE 0

#define S_OK 0
#define S_FALSE 1
#define E_FAIL -1
#define E_NOTIMPL -2
#define E_OUTOFMEMORY -3
#define E_INVALIDARG -4
#define E_POINTER -5
#define E_HANDLE -6
#define E_ABORT -7
#define E_ACCESSDENIED -8
#define E_PENDING -9

#define WINAPI
#define CALLBACK
#define __stdcall
#define STDMETHODCALLTYPE
#define APIENTRY
#define CONST const
#define OUT
#define FAR
#define __forceinline inline __attribute__((always_inline))
#define _MAX_PATH 260
#define MAX_PATH 260
#define stricmp strcasecmp
#define _stricmp strcasecmp
#define _strnicmp strncasecmp
#define strnicmp strncasecmp
#define _vsnprintf vsnprintf
#ifndef _strupr
static inline char* _strupr(char* text) {
    if (!text) return text;
    for (char* p = text; *p; ++p) *p = (char)toupper((unsigned char)*p);
    return text;
}
#endif
#define _snprintf snprintf
#define MB_OK 0
#define MB_TOPMOST 0x00040000L
#define CP_ACP 0
#define CP_UTF8 65001

// Small Win32-compatible text conversion layer. Android stores wchar_t as
// UTF-32; UTF-8 is decoded explicitly and legacy single-byte code pages use a
// lossless byte-to-codepoint mapping until their dedicated tables are wired.
static inline int MultiByteToWideChar(UINT codePage, DWORD, LPCSTR src,
                                     int srcLen, LPWSTR dst, int dstLen) {
    if (!src) return 0;
    const BOOL includeNull = srcLen < 0;
    const size_t inputLen = includeNull ? strlen(src) : (size_t)srcLen;
    size_t i = 0;
    int required = 0;
    while (i < inputLen) {
        uint32_t cp;
        const unsigned char c = (unsigned char)src[i];
        size_t consumed = 1;
        if (codePage == CP_UTF8 && c >= 0x80) {
            if ((c & 0xE0) == 0xC0 && i + 1 < inputLen &&
                (((unsigned char)src[i + 1] & 0xC0) == 0x80)) {
                cp = ((uint32_t)(c & 0x1F) << 6) |
                     ((uint32_t)src[i + 1] & 0x3F);
                consumed = 2;
            } else if ((c & 0xF0) == 0xE0 && i + 2 < inputLen &&
                       (((unsigned char)src[i + 1] & 0xC0) == 0x80) &&
                       (((unsigned char)src[i + 2] & 0xC0) == 0x80)) {
                cp = ((uint32_t)(c & 0x0F) << 12) |
                     (((uint32_t)src[i + 1] & 0x3F) << 6) |
                     ((uint32_t)src[i + 2] & 0x3F);
                consumed = 3;
            } else if ((c & 0xF8) == 0xF0 && i + 3 < inputLen &&
                       (((unsigned char)src[i + 1] & 0xC0) == 0x80) &&
                       (((unsigned char)src[i + 2] & 0xC0) == 0x80) &&
                       (((unsigned char)src[i + 3] & 0xC0) == 0x80)) {
                cp = ((uint32_t)(c & 0x07) << 18) |
                     (((uint32_t)src[i + 1] & 0x3F) << 12) |
                     (((uint32_t)src[i + 2] & 0x3F) << 6) |
                     ((uint32_t)src[i + 3] & 0x3F);
                consumed = 4;
            } else {
                cp = 0xFFFD;
            }
        } else {
            cp = c;
        }
        if (dst && required < dstLen) dst[required] = (wchar_t)cp;
        ++required;
        i += consumed;
    }
    if (includeNull) {
        if (dst && required < dstLen) dst[required] = 0;
        ++required;
    }
    return (!dst || required <= dstLen) ? required : 0;
}

static inline int WideCharToMultiByte(UINT codePage, DWORD, LPCWSTR src,
                                     int srcLen, LPSTR dst, int dstLen,
                                     LPCSTR defaultChar, LPBOOL usedDefault) {
    if (!src) return 0;
    const BOOL includeNull = srcLen < 0;
    const size_t inputLen = includeNull ? wcslen(src) : (size_t)srcLen;
    int required = 0;
    if (usedDefault) *usedDefault = FALSE;
    for (size_t i = 0; i < inputLen; ++i) {
        uint32_t cp = (uint32_t)src[i];
        unsigned char bytes[4];
        int count;
        if (codePage == CP_UTF8) {
            if (cp <= 0x7F) { bytes[0] = (unsigned char)cp; count = 1; }
            else if (cp <= 0x7FF) {
                bytes[0] = (unsigned char)(0xC0 | (cp >> 6));
                bytes[1] = (unsigned char)(0x80 | (cp & 0x3F)); count = 2;
            } else if (cp <= 0xFFFF) {
                bytes[0] = (unsigned char)(0xE0 | (cp >> 12));
                bytes[1] = (unsigned char)(0x80 | ((cp >> 6) & 0x3F));
                bytes[2] = (unsigned char)(0x80 | (cp & 0x3F)); count = 3;
            } else {
                bytes[0] = (unsigned char)(0xF0 | (cp >> 18));
                bytes[1] = (unsigned char)(0x80 | ((cp >> 12) & 0x3F));
                bytes[2] = (unsigned char)(0x80 | ((cp >> 6) & 0x3F));
                bytes[3] = (unsigned char)(0x80 | (cp & 0x3F)); count = 4;
            }
        } else {
            if (cp <= 0xFF) bytes[0] = (unsigned char)cp;
            else {
                bytes[0] = (unsigned char)((defaultChar && *defaultChar) ? *defaultChar : '?');
                if (usedDefault) *usedDefault = TRUE;
            }
            count = 1;
        }
        if (dst && required + count <= dstLen)
            for (int j = 0; j < count; ++j) dst[required + j] = (char)bytes[j];
        required += count;
    }
    if (includeNull) {
        if (dst && required < dstLen) dst[required] = 0;
        ++required;
    }
    return (!dst || required <= dstLen) ? required : 0;
}
#define FW_NORMAL 400
#define FW_BOLD 700
#define OUT_DEFAULT_PRECIS 0
#define CLIP_DEFAULT_PRECIS 0
#define ANTIALIASED_QUALITY 4
#define DEFAULT_PITCH 0
#define BLACK_BRUSH 4
#define WS_OVERLAPPEDWINDOW 0
#define WS_OVERLAPPED 0
#define WS_CAPTION 0
#define WS_SYSMENU 0
#define WS_MINIMIZEBOX 0
#define WS_POPUP 0
#define HWND_TOP ((HWND)0)
#define HWND_TOPMOST ((HWND)(intptr_t)-1)
#define SWP_SHOWWINDOW 0
#define INVALID_HANDLE_VALUE ((HANDLE)(intptr_t)-1)
#define FILE_ATTRIBUTE_DIRECTORY 0x00000010
#define FILE_ATTRIBUTE_NORMAL 0x00000080
#define TIMERR_NOERROR 0
#define MAKEFOURCC(ch0, ch1, ch2, ch3) \
  ((DWORD)(BYTE)(ch0) | ((DWORD)(BYTE)(ch1) << 8) | \
   ((DWORD)(BYTE)(ch2) << 16) | ((DWORD)(BYTE)(ch3) << 24))
#define VK_LEFT 0x25
#define VK_TAB 0x09
#define VK_RETURN 0x0D
#define VK_ESCAPE 0x1B
#define VK_SCROLL 0x91
#define VK_UP 0x26
#define VK_RIGHT 0x27
#define VK_DOWN 0x28
#define VK_HOME 0x24
#define VK_END 0x23
#define VK_DELETE 0x2E
#define DIK_ESCAPE 0x01
#define DIK_ESC DIK_ESCAPE
#define DIK_1 0x02
#define DIK_2 0x03
#define DIK_3 0x04
#define DIK_4 0x05
#define DIK_5 0x06
#define DIK_6 0x07
#define DIK_7 0x08
#define DIK_8 0x09
#define DIK_9 0x0A
#define DIK_0 0x0B
#define DIK_MINUS 0x0C
#define DIK_EQUALS 0x0D
#define DIK_BACK 0x0E
#define DIK_TAB 0x0F
#define DIK_Q 0x10
#define DIK_W 0x11
#define DIK_E 0x12
#define DIK_R 0x13
#define DIK_T 0x14
#define DIK_Y 0x15
#define DIK_U 0x16
#define DIK_I 0x17
#define DIK_O 0x18
#define DIK_P 0x19
#define DIK_LBRACKET 0x1A
#define DIK_RBRACKET 0x1B
#define DIK_RETURN 0x1C
#define DIK_LCONTROL 0x1D
#define DIK_A 0x1E
#define DIK_S 0x1F
#define DIK_D 0x20
#define DIK_F 0x21
#define DIK_G 0x22
#define DIK_H 0x23
#define DIK_J 0x24
#define DIK_K 0x25
#define DIK_L 0x26
#define DIK_SEMICOLON 0x27
#define DIK_APOSTROPHE 0x28
#define DIK_GRAVE 0x29
#define DIK_LSHIFT 0x2A
#define DIK_BACKSLASH 0x2B
#define DIK_Z 0x2C
#define DIK_X 0x2D
#define DIK_C 0x2E
#define DIK_V 0x2F
#define DIK_B 0x30
#define DIK_N 0x31
#define DIK_M 0x32
#define DIK_COMMA 0x33
#define DIK_PERIOD 0x34
#define DIK_SLASH 0x35
#define DIK_RSHIFT 0x36
#define DIK_MULTIPLY 0x37
#define DIK_LMENU 0x38
#define DIK_LALT DIK_LMENU
#define DIK_SPACE 0x39
#define DIK_CAPITAL 0x3A
#define DIK_F1 0x3B
#define DIK_F2 0x3C
#define DIK_F3 0x3D
#define DIK_F4 0x3E
#define DIK_F5 0x3F
#define DIK_F6 0x40
#define DIK_F7 0x41
#define DIK_F8 0x42
#define DIK_F9 0x43
#define DIK_F10 0x44
#define DIK_NUMLOCK 0x45
#define DIK_SCROLL 0x46
#define DIK_NUMPAD7 0x47
#define DIK_NUMPAD8 0x48
#define DIK_NUMPAD9 0x49
#define DIK_SUBTRACT 0x4A
#define DIK_NUMPAD4 0x4B
#define DIK_NUMPAD5 0x4C
#define DIK_NUMPAD6 0x4D
#define DIK_ADD 0x4E
#define DIK_NUMPAD1 0x4F
#define DIK_NUMPAD2 0x50
#define DIK_NUMPAD3 0x51
#define DIK_NUMPAD0 0x52
#define DIK_DECIMAL 0x53
#define DIK_F11 0x57
#define DIK_F12 0x58
#define DIK_NUMPADENTER 0x9C
#define DIK_RCONTROL 0x9D
#define DIK_DIVIDE 0xB5
#define DIK_SYSRQ 0xB7
#define DIK_RMENU 0xB8
#define DIK_RALT DIK_RMENU
#define DIK_PAUSE 0xC5
#define DIK_HOME 0xC7
#define DIK_UP 0xC8
#define DIK_PRIOR 0xC9
#define DIK_PGUP DIK_PRIOR
#define DIK_LEFT 0xCB
#define DIK_RIGHT 0xCD
#define DIK_END 0xCF
#define DIK_DOWN 0xD0
#define DIK_NEXT 0xD1
#define DIK_PGDN DIK_NEXT
#define DIK_INSERT 0xD2
#define DIK_DELETE 0xD3
#define DIK_LWIN 0xDB
#define DIK_RWIN 0xDC
#define DIK_APPS 0xDD
#define DIK_MUTE 0xA0
#define DIK_VOLUMEDOWN 0xAE
#define DIK_VOLUMEUP 0xB0
#define DIK_NEXTTRACK 0x99
#define DIK_MEDIASTOP 0xA4
#define DIK_PLAYPAUSE 0xA2
#define DIK_CALCULATOR 0xA1
#define DIK_WEBHOME 0xB2
#define DIK_NUMPADCOMMA 0xB3
#define _PC_24 0
#define _MCW_PC 0
#define _alloca alloca
#define _access access
#define _unlink unlink
#define _getcwd getcwd
#define _chmod chmod
#define _S_IWRITE S_IWUSR
#define GMEM_FIXED 0
#define GMEM_ZEROINIT 0x0040
#define GMEM_MOVEABLE 0x0002
#define KL_NAMELENGTH 9
#define _TRUNCATE ((size_t)-1)
#define _countof(array) (sizeof(array) / sizeof((array)[0]))
#define ZeroMemory(destination, length) memset((destination), 0, (length))

static inline int strncpy_s(char* destination, size_t destinationSize,
                            const char* source, size_t count) {
  if (!destination || destinationSize == 0) return 22;
  if (!source) { destination[0] = '\0'; return 22; }
  size_t sourceLength = strlen(source);
  size_t copyLength = (count == _TRUNCATE || count > sourceLength) ? sourceLength : count;
  if (copyLength >= destinationSize) copyLength = destinationSize - 1;
  memcpy(destination, source, copyLength);
  destination[copyLength] = '\0';
  return 0;
}

inline int MessageBox(HWND, const char*, const char*, unsigned int) { return 0; }
inline BOOL AllocConsole() { return TRUE; }
inline HRESULT CoInitialize(LPVOID) { return S_OK; }
inline void CoUninitialize() {}
inline void PostQuitMessage(int) {}
inline short GetAsyncKeyState(int) { return 0; }
inline HDC GetDC(HWND) { return NULL; }
inline int ReleaseDC(HWND, HDC) { return 1; }
inline void Sleep(DWORD milliseconds) { usleep((useconds_t)milliseconds * 1000u); }
inline DWORD timeGetTime()
{
  struct timespec now;
  clock_gettime(CLOCK_MONOTONIC, &now);
  return (DWORD)((uint64_t)now.tv_sec * 1000ULL + (uint64_t)now.tv_nsec / 1000000ULL);
}
inline DWORD GetTickCount() { return timeGetTime(); }
inline DWORD GetCurrentTime() { return timeGetTime(); }
inline BOOL SetWindowPos(HWND, HWND, int, int, int, int, UINT) { return TRUE; }
inline unsigned int _controlfp(unsigned int value, unsigned int) { return value; }
inline HANDLE FindFirstFile(const char*, WIN32_FIND_DATA*) { return INVALID_HANDLE_VALUE; }
inline BOOL FindNextFile(HANDLE, WIN32_FIND_DATA*) { return FALSE; }
inline BOOL FindClose(HANDLE) { return TRUE; }
inline HMODULE GetModuleHandle(const char*) { return NULL; }
inline DWORD GetModuleFileName(HMODULE, char* output, DWORD outputSize)
{
  if (!output || outputSize == 0) return 0;
#ifdef __ANDROID__
  ssize_t count = readlink("/proc/self/exe", output, outputSize - 1);
  if (count < 0) { output[0] = '\0'; return 0; }
  output[count] = '\0';
  return (DWORD)count;
#else
  // iOS does not expose /proc. The mobile launcher changes into the app's
  // writable data directory before the legacy engine starts, so its current
  // directory is the correct module base for all relative client paths.
  if (!getcwd(output, outputSize)) { output[0] = '\0'; return 0; }
  return (DWORD)strlen(output);
#endif
}
inline BOOL CreateDirectory(const char* path, LPSECURITY_ATTRIBUTES)
{
  if (!path || !*path) return FALSE;
  return (mkdir(path, 0755) == 0 || errno == EEXIST) ? TRUE : FALSE;
}
inline BOOL RemoveDirectory(const char* path) { return path && rmdir(path) == 0 ? TRUE : FALSE; }
inline HANDLE GlobalAlloc(UINT flags, SIZE_T size)
{
  void* memory = (flags & GMEM_ZEROINIT) ? calloc(1, size) : malloc(size);
  return (HANDLE)memory;
}
inline HANDLE GlobalFree(HANDLE memory) { free(memory); return NULL; }
inline BOOL MoveFile(const char* oldPath, const char* newPath)
{
  if (!oldPath || !newPath) return FALSE;
  if (strcmp(oldPath, newPath) == 0) return TRUE;
  return rename(oldPath, newPath) == 0 ? TRUE : FALSE;
}
inline BOOL CopyFile(const char* source, const char* destination, BOOL failIfExists)
{
  if (!source || !destination) return FALSE;
  if (failIfExists && access(destination, F_OK) == 0) return FALSE;
  FILE* input = fopen(source, "rb");
  if (!input) return FALSE;
  FILE* output = fopen(destination, "wb");
  if (!output) { fclose(input); return FALSE; }
  char buffer[64 * 1024];
  size_t count;
  BOOL ok = TRUE;
  while ((count = fread(buffer, 1, sizeof(buffer), input)) != 0)
  {
    if (fwrite(buffer, 1, count, output) != count) { ok = FALSE; break; }
  }
  if (ferror(input)) ok = FALSE;
  if (fclose(output) != 0) ok = FALSE;
  fclose(input);
  if (!ok) unlink(destination);
  return ok;
}
inline BOOL DeleteFile(const char* path) { return path && unlink(path) == 0 ? TRUE : FALSE; }
inline DWORD GetCurrentDirectory(DWORD size, char* buffer)
{
  if (!buffer || size == 0 || !getcwd(buffer, size)) return 0;
  return (DWORD)strlen(buffer);
}
inline int lstrlen(const char* value) { return value ? (int)strlen(value) : 0; }
inline DWORD GetPrivateProfileString(const char*, const char*, const char* defaultValue,
                                     char* output, DWORD outputSize, const char*)
{
  if (!output || outputSize == 0) return 0;
  const char* value = defaultValue ? defaultValue : "";
  strncpy_s(output, outputSize, value, _TRUNCATE);
  return (DWORD)strlen(output);
}
inline int LoadString(HINSTANCE, UINT, char* output, int outputSize)
{
  if (output && outputSize > 0) output[0] = '\0';
  return 0;
}
inline HANDLE GetCurrentProcess() { return (HANDLE)(intptr_t)getpid(); }
inline UINT timeGetDevCaps(TIMECAPS* caps, UINT)
{
  if (caps) { caps->wPeriodMin = 1; caps->wPeriodMax = 1; }
  return 0;
}
inline UINT timeBeginPeriod(UINT) { return 0; }
inline UINT timeEndPeriod(UINT) { return 0; }
inline HANDLE CreateMutex(LPSECURITY_ATTRIBUTES, BOOL, const char*)
{
  pthread_mutex_t* mutex = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
  if (!mutex) return NULL;
  pthread_mutex_init(mutex, NULL);
  return (HANDLE)mutex;
}
inline BOOL ReleaseMutex(HANDLE handle)
{
  if (!handle) return FALSE;
  pthread_mutex_unlock((pthread_mutex_t*)handle);
  return TRUE;
}

typedef struct _RECT {
  LONG left;
  LONG top;
  LONG right;
  LONG bottom;
} RECT, *PRECT;

typedef struct _POINT {
  LONG x;
  LONG y;
} POINT, *PPOINT;
typedef POINT* LPPOINT;
inline BOOL GetCursorPos(LPPOINT point) { if (point) { point->x = 0; point->y = 0; } return TRUE; }
inline BOOL ScreenToClient(HWND, LPPOINT) { return TRUE; }
inline int16_t GetKeyState(int) { return 0; }
inline BOOL SetFileAttributes(const char*, DWORD) { return TRUE; }
inline LPSTR CharNextExA(WORD codePage, LPCSTR current, DWORD)
{
  if (!current || !*current) return const_cast<LPSTR>(current);
  size_t length = 1;
  if (codePage == CP_UTF8) {
    const unsigned char lead = (unsigned char)*current;
    if ((lead & 0xE0) == 0xC0) length = 2;
    else if ((lead & 0xF0) == 0xE0) length = 3;
    else if ((lead & 0xF8) == 0xF0) length = 4;
  }
  return const_cast<LPSTR>(current + length);
}
inline LPSTR CharPrevExA(WORD codePage, LPCSTR start, LPCSTR current, DWORD)
{
  if (!start || !current || current <= start) return const_cast<LPSTR>(start);
  LPCSTR previous = start;
  for (LPCSTR cursor = start; cursor < current;) {
    previous = cursor;
    LPSTR next = CharNextExA(codePage, cursor, 0);
    if (next <= cursor || next >= current) break;
    cursor = next;
  }
  return const_cast<LPSTR>(previous);
}

typedef struct _SIZE {
  LONG cx;
  LONG cy;
} SIZE, *PSIZE;

typedef struct _ABCFLOAT {
  FLOAT abcfA;
  FLOAT abcfB;
  FLOAT abcfC;
} ABCFLOAT;

#ifndef BI_RGB
#define BI_RGB 0
#define DIB_RGB_COLORS 0
#define TRANSPARENT 1
#define RGB(r,g,b) ((DWORD)(((BYTE)(r)) | ((WORD)((BYTE)(g)) << 8) | (((DWORD)(BYTE)(b)) << 16)))
#endif

typedef struct _LUPUS2_GDI_OBJECT {
  DWORD magic;
  void* pixels;
} LUPUS2_GDI_OBJECT;

static inline HDC CreateCompatibleDC(HDC) { return calloc(1, 1); }
static inline BOOL DeleteDC(HDC dc) { free(dc); return TRUE; }
static inline HBITMAP CreateDIBSection(HDC, const BITMAPINFO* info, UINT, void** bits, HANDLE, DWORD) {
  LUPUS2_GDI_OBJECT* object = (LUPUS2_GDI_OBJECT*)calloc(1, sizeof(LUPUS2_GDI_OBJECT));
  if (!object) return NULL;
  object->magic = 0x424d5041u;
  const size_t width = (size_t)(info->bmiHeader.biWidth < 0 ? -info->bmiHeader.biWidth : info->bmiHeader.biWidth);
  const size_t height = (size_t)(info->bmiHeader.biHeight < 0 ? -info->bmiHeader.biHeight : info->bmiHeader.biHeight);
  object->pixels = calloc(width * height, 4);
  if (!object->pixels) { free(object); return NULL; }
  *bits = object->pixels;
  return (HBITMAP)object;
}
static inline HFONT CreateFontIndirect(const LOGFONT*) {
  LUPUS2_GDI_OBJECT* object = (LUPUS2_GDI_OBJECT*)calloc(1, sizeof(LUPUS2_GDI_OBJECT));
  if (object) object->magic = 0x464f4e54u;
  return (HFONT)object;
}
static inline BOOL DeleteObject(HGDIOBJ handle) {
  LUPUS2_GDI_OBJECT* object = (LUPUS2_GDI_OBJECT*)handle;
  if (!object) return FALSE;
  if (object->magic == 0x424d5041u) free(object->pixels);
  free(object);
  return TRUE;
}
static inline HGDIOBJ SelectObject(HDC, HGDIOBJ object) { return object; }
static inline DWORD SetTextColor(HDC, DWORD color) { return color; }
static inline DWORD SetBkColor(HDC, DWORD color) { return color; }
static inline int SetBkMode(HDC, int mode) { return mode; }
static inline BOOL TextOut(HDC, int, int, LPCSTR, int) { return TRUE; }
static inline BOOL TextOutW(HDC, int, int, LPCWSTR, int) { return TRUE; }
static inline int SetDIBitsToDevice(HDC, int, int, DWORD, DWORD, int, int, UINT, UINT, const void*, const BITMAPINFO*, UINT) { return 1; }
static inline BOOL GetTextExtentPoint32W(HDC, LPCWSTR, int count, SIZE* size) { size->cx = 8 * count; size->cy = 16; return TRUE; }
static inline BOOL GetTextExtentPoint32(HDC, LPCSTR, int count, SIZE* size) { size->cx = 8 * count; size->cy = 16; return TRUE; }
static inline BOOL GetCharABCWidthsFloatW(HDC, UINT, UINT, ABCFLOAT* abc) { abc->abcfA = 0.0f; abc->abcfB = 8.0f; abc->abcfC = 0.0f; return TRUE; }

#ifdef __cplusplus
}

struct IBaseFilter;
struct ISampleGrabber;
struct IMediaControl;
struct IMediaEventEx;
struct IVideoWindow;
struct IBasicVideo;
#endif
