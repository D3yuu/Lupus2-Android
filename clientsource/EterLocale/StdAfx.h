// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#pragma once

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#if defined(__ANDROID__) || defined(__IOS__)
#include "../android_compat/windows.h"
#else
#include <windows.h>
#endif
#include <assert.h>
#include <cstddef>
#include <cctype>

#include "CodePageId.h"

#include "../UserInterface/Locale_inc.h"
