//
// PocketTextElite - port of Elite[TM] trading system
// Copyright (C) 2008-2009 Michael Fink
//
/// \file stdafx.h Precompiled header support
//
#pragma once

#ifdef UNDER_CE
// The WTL wizard was instructed to create this project as a COM Server.
// On Windows CE, COM Servers are only available on platforms that include DCOM.
// Pocket PC 2000, 2002, 2003 and SmarthPhone 2002, 2003 do not include DCOM.
// The Standard SDK for Windows CE versions 3.0, 4.0, 4.1, and 4.2 do not include DCOM.
// For Windows CE OSes released after 2003, please see the associated docs.

// Change these values to use different versions
#define WINVER      0x0420

#define _WIN32_WCE_AYGSHELL 1

#include <atlbase.h>

#undef min
#undef max
#define min(x,y) ((x)<(y)?(x):(y))
#define max(x,y) ((x)>(y)?(x):(y))

#include <atlapp.h>

extern CAppModule _Module;

#include <atlmisc.h>
#include <atlwin.h>

#include <atlframe.h>
#include <atlctrls.h>
#include <atldlgs.h>

#include <aygshell.h>
#pragma comment(lib, "aygshell.lib")

#include <atlddx.h>

#define _WTL_CE_NO_ZOOMSCROLL
#include <atlctrlx.h>
#include <atlwince.h>

//#undef min
//#undef max

//{{AFX_INSERT_LOCATION}}
// Microsoft eMbedded Visual C++ will insert additional declarations immediately before the previous line.

#else
// Change these values to use different versions
#define WINVER      0x0500
#define _WIN32_WINNT   0x0501
#define _WIN32_IE   0x0501
#define _RICHEDIT_VER   0x0200

#define _CRT_SECURE_NO_WARNINGS

#include <atlbase.h>
#include <atlapp.h>

extern CAppModule _Module;

#include <atlwin.h>
#include <atlmisc.h>

#include <atlframe.h>
#include <atlctrls.h>
#include <atldlgs.h>
#include <atlctrlx.h>
#include <atlddx.h>

#if defined _M_IX86
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif

#endif
