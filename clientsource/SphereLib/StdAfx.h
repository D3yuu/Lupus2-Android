#pragma once

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
//#define SPHERELIB_STRICT
//#include <crtdbg.h>

// Pull the platform compatibility types (DWORD, LONG, HWND, BOOL, ...)
// before the Direct3D-on-OpenGL compatibility declarations.
#include "../eterBase/StdAfx.h"

#ifdef ENABLE_OPENGL
#include "../eterLib/GrpOpenGL.h"
#else
#include <d3d8.h>
#include <d3dx8.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "../UserInterface/Locale_inc.h"
