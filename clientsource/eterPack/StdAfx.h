#pragma once

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include "../UserInterface/Locale_inc.h"
//#include <crtdbg.h>
#if !defined(__ANDROID__) && !defined(__IOS__)
#include <windows.h>
#endif
#include <assert.h>

#include "../eterBase/StdAfx.h"
