//
// Recorder - a GPS logger app for Windows Mobile
// Copyright (C) 2006-2019 Michael Fink
//
/// \file stdafx.h Precompiled header support
//
#pragma once

#ifdef _WIN32_WCE

// Change this value to use different versions
#define WINVER 0x0420

// WinCE
#include <aygshell.h>
#pragma comment(lib, "aygshell.lib")

// ATL
#define _SECURE_ATL 1
#define _WIN32_WCE_AYGSHELL 1
#include <atlbase.h>

#include <atlstr.h>
#include <atltypes.h>
#include <atlsync.h>
#include <atlcomtime.h>

// WTL
#define _WTL_NO_CSTRING

#include <atlapp.h>

extern CAppModule _Module;

#include <atlwin.h>
#include <atlframe.h>
#include <atlctrls.h>
#include <atldlgs.h>

#define _WTL_CE_NO_ZOOMSCROLL
#define _WTL_CE_NO_FULLSCREEN
#include <atlwince.h>

#else

// ATL
#define _SECURE_ATL 1
#include <atlbase.h>

#include <atlstr.h>
#include <atltypes.h>
#include <atlsync.h>
#include <atlcomtime.h>

// WTL
#define _WTL_NO_CSTRING

#include <atlapp.h>

extern CAppModule _Module;

#include <atlwin.h>
#include <atlframe.h>
#include <atlctrls.h>
#include <atldlgs.h>

#include "WinCECompat.hpp"

#endif // _WIN32_WCE

#include <stdexcept>
#include <exception>
#include <boost/shared_ptr.hpp>
