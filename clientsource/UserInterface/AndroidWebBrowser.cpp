#include "StdAfx.h"
#include "../CWebBrowser/CWebBrowser.h"

static RECT s_rect = {0, 0, 0, 0};
extern "C" {
int WebBrowser_Startup(HINSTANCE) { return 1; }
void WebBrowser_Cleanup() {}
void WebBrowser_Destroy() {}
int WebBrowser_Show(HWND, const char*, const RECT* rect) { if (rect) s_rect = *rect; return 0; }
void WebBrowser_Hide() {}
void WebBrowser_Move(const RECT* rect) { if (rect) s_rect = *rect; }
int WebBrowser_IsVisible() { return 0; }
const RECT& WebBrowser_GetRect() { return s_rect; }
}
