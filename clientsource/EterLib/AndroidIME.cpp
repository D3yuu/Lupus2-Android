#include "StdAfx.h"
#include "IME.h"

int CIME::ms_compLen = 0;
int CIME::ms_curpos = 0;
int CIME::ms_lastpos = 0;
int CIME::ms_ulbegin = 0;
int CIME::ms_ulend = 0;
wchar_t CIME::m_wText[IMESTR_MAXLEN] = {};
bool CIME::ms_bInitialized = false;
bool CIME::ms_bDisableIMECompletely = false;
bool CIME::ms_bUILessMode = false;
bool CIME::ms_bImeEnabled = true;
bool CIME::ms_bCaptureInput = false;
bool CIME::ms_bChineseIME = false;
bool CIME::ms_bUseIMMCandidate = false;
HWND CIME::ms_hWnd = NULL;
HKL CIME::ms_hklCurrent = NULL;
char CIME::ms_szKeyboardLayout[KL_NAMELENGTH + 1] = {};
OSVERSIONINFOA CIME::ms_stOSVI = {};
HINSTANCE CIME::ms_hImm32Dll = NULL;
HINSTANCE CIME::ms_hCurrentImeDll = NULL;
DWORD CIME::ms_dwImeState = 0;
DWORD CIME::ms_adwId[2] = {};
DWORD CIME::ms_dwIMELevel = 0;
DWORD CIME::ms_dwIMELevelSaved = 0;
bool CIME::ms_bCandidateList = false;
DWORD CIME::ms_dwCandidateCount = 0;
bool CIME::ms_bVerticalCandidate = false;
int CIME::ms_iCandListIndexBase = 0;
WCHAR CIME::ms_wszCandidate[CIME::MAX_CANDLIST][MAX_CANDIDATE_LENGTH] = {};
DWORD CIME::ms_dwCandidateSelection = 0;
DWORD CIME::ms_dwCandidatePageSize = 0;
bool CIME::ms_bReadingInformation = false;
int CIME::ms_iReadingError = 0;
bool CIME::ms_bHorizontalReading = false;
std::vector<wchar_t> CIME::ms_wstrReading;
wchar_t* CIME::ms_wszCurrentIndicator = NULL;
IIMEEventSink* CIME::ms_pEvent = NULL;
UINT CIME::ms_uOutputCodePage = CP_UTF8;
UINT CIME::ms_uInputCodePage = CP_UTF8;
INPUTCONTEXT* (WINAPI* CIME::_ImmLockIMC)(HIMC) = NULL;
BOOL (WINAPI* CIME::_ImmUnlockIMC)(HIMC) = NULL;
LPVOID (WINAPI* CIME::_ImmLockIMCC)(HIMCC) = NULL;
BOOL (WINAPI* CIME::_ImmUnlockIMCC)(HIMCC) = NULL;
UINT (WINAPI* CIME::_GetReadingString)(HIMC, UINT, LPWSTR, PINT, BOOL*, PUINT) = NULL;
BOOL (WINAPI* CIME::_ShowReadingWindow)(HIMC, BOOL) = NULL;

CIME::CIME() : m_hOrgIMC(NULL), m_max(IMESTR_MAXLEN - 1), m_userMax(IMESTR_MAXLEN - 1),
    m_bOnlyNumberMode(FALSE), m_bEnablePaste(true), m_bUseDefaultIME(true)
{
    m_wszComposition[0] = 0;
}
CIME::~CIME() { Uninitialize(); }
bool CIME::Initialize(HWND window) { ms_hWnd = window; ms_bInitialized = true; Clear(); return true; }
void CIME::Uninitialize() { ms_bInitialized = false; ms_hWnd = NULL; }
void CIME::Clear() { m_wText[0] = 0; ms_curpos = ms_lastpos = ms_compLen = ms_ulbegin = ms_ulend = 0; }
void CIME::SetMax(int value) { m_max = value; }
void CIME::SetUserMax(int value) { m_userMax = value; }
void CIME::SetText(const char* text, int len)
{
    if (!text) { Clear(); return; }
    std::string source(text, len >= 0 ? (size_t)len : strlen(text));
    int count = MultiByteToWideChar(CP_UTF8, 0, source.c_str(), (int)source.size(), m_wText, IMESTR_MAXLEN - 1);
    ms_lastpos = ms_curpos = count > 0 ? count : 0;
    m_wText[ms_lastpos] = 0;
}
int CIME::GetText(std::string& text, bool)
{
    int bytes = WideCharToMultiByte(CP_UTF8, 0, m_wText, ms_lastpos, NULL, 0, NULL, NULL);
    text.resize(bytes > 0 ? bytes : 0);
    if (bytes > 0) WideCharToMultiByte(CP_UTF8, 0, m_wText, ms_lastpos, &text[0], bytes, NULL, NULL);
    return (int)text.size();
}
const char* CIME::GetCodePageText() { return "UTF-8"; }
int CIME::GetCodePage() { return CP_UTF8; }
int CIME::GetCandidateCount() { return 0; }
int CIME::GetCandidatePageCount() { return 0; }
int CIME::GetCandidate(DWORD, std::string& text) { text.clear(); return 0; }
int CIME::GetCandidateSelection() { return -1; }
int CIME::GetReading(std::string& text) { text.clear(); return 0; }
int CIME::GetReadingError() { return 0; }
void CIME::SetInputMode(DWORD mode) { ms_dwImeState = mode; }
DWORD CIME::GetInputMode() { return ms_dwImeState; }
bool CIME::IsIMEEnabled() { return ms_bImeEnabled; }
void CIME::EnableIME(bool enable) { ms_bImeEnabled = enable; }
void CIME::DisableIME() { ms_bImeEnabled = false; }
void CIME::EnableCaptureInput() { ms_bCaptureInput = true; }
void CIME::DisableCaptureInput() { ms_bCaptureInput = false; }
bool CIME::IsCaptureEnabled() { return ms_bCaptureInput; }
void CIME::SetNumberMode() { m_bOnlyNumberMode = TRUE; }
void CIME::SetStringMode() { m_bOnlyNumberMode = FALSE; }
bool CIME::__IsWritable(wchar_t key) { return !m_bOnlyNumberMode || (key >= L'0' && key <= L'9'); }
void CIME::AddExceptKey(wchar_t key) { m_exceptKey.push_back(key); }
void CIME::ClearExceptKey() { m_exceptKey.clear(); }
void CIME::PasteTextFromClipBoard() {}
void CIME::EnablePaste(bool enable) { m_bEnablePaste = enable; }
void CIME::PasteString(const char* text) { if (m_bEnablePaste && text) SetText(text, (int)strlen(text)); }
void CIME::FinalizeString(bool) { ms_compLen = 0; }
void CIME::UseDefaultIME() { m_bUseDefaultIME = true; }
int CIME::GetCurPos() { return ms_curpos; }
int CIME::GetCompLen() { return ms_compLen; }
int CIME::GetULBegin() { return ms_ulbegin; }
int CIME::GetULEnd() { return ms_ulend; }
void CIME::CloseCandidateList() { ms_bCandidateList = false; }
void CIME::CloseReadingInformation() { ms_bReadingInformation = false; }
void CIME::ChangeInputLanguage() {}
void CIME::ChangeInputLanguageWorker() {}
LRESULT CIME::WMInputLanguage(HWND, UINT, WPARAM, LPARAM) { return 0; }
LRESULT CIME::WMStartComposition(HWND, UINT, WPARAM, LPARAM) { return 0; }
LRESULT CIME::WMComposition(HWND, UINT, WPARAM, LPARAM) { return 0; }
LRESULT CIME::WMEndComposition(HWND, UINT, WPARAM, LPARAM) { return 0; }
LRESULT CIME::WMNotify(HWND, UINT, WPARAM, LPARAM) { return 0; }
LRESULT CIME::WMChar(HWND, UINT, WPARAM key, LPARAM param)
{
    const wchar_t character = static_cast<wchar_t>(key);

    // Android sends Backspace through WM_CHAR.  It must modify the IME buffer
    // directly; inserting character 0x08 made password fields gain a bullet
    // instead of deleting the previous character.
    if (character == 8)
    {
        if (!ms_bCaptureInput)
            return 0;
        if (ms_curpos > 0)
        {
            DecCurPos();
            DelCurPos();
        }
        if (ms_pEvent)
            ms_pEvent->OnUpdate();
        return 0;
    }

    // Return/Tab/Escape are actions handled by CPythonIME, not text.
    if (ms_pEvent && ms_pEvent->OnWM_CHAR(key, param))
        return 0;
    if (!ms_bCaptureInput)
        return 0;

    OnChar(character);
    // Keep the focused Python EditLine synchronized on every keystroke.  The
    // previous Android stub updated only its private buffer, so the text became
    // visible only after the field lost focus or Connect was pressed.
    if (ms_pEvent)
        ms_pEvent->OnUpdate();
    return 0;
}
void CIME::IncCurPos() { if (ms_curpos < ms_lastpos) ++ms_curpos; }
void CIME::DecCurPos() { if (ms_curpos > 0) --ms_curpos; }
void CIME::SetCurPos(int offset)
{
    // EditLine uses -1 when it wants the caret at the end.  Preserve that PC
    // client convention on Android instead of clamping it to position zero.
    ms_curpos = offset < 0 ? ms_lastpos : std::max(0, std::min(offset, ms_lastpos));
}
void CIME::DelCurPos() { if (ms_curpos < ms_lastpos) { memmove(m_wText + ms_curpos, m_wText + ms_curpos + 1, (ms_lastpos - ms_curpos) * sizeof(wchar_t)); --ms_lastpos; } }
void CIME::InsertString(wchar_t* text, int size) { for (int i = 0; i < size; ++i) OnChar(text[i]); }
void CIME::OnChar(wchar_t c)
{
    if (!__IsWritable(c) || ms_lastpos >= std::min(m_max, m_userMax)) return;
    memmove(m_wText + ms_curpos + 1, m_wText + ms_curpos, (ms_lastpos - ms_curpos + 1) * sizeof(wchar_t));
    m_wText[ms_curpos++] = c; ++ms_lastpos;
}
bool CIME::IsMax(const wchar_t*, int len) { return len >= std::min(m_max, m_userMax); }
