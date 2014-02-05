//
// ExifShellExt - Exif data shell extension
// Copyright (C) 2008 Michael Fink
//
/// \file stdafx.h Precompiled header support
//
#pragma once

#define WINVER _WIN32_WINNT_WINXP

// exclude rarely-used stuff from Windows headers
#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN

// no min-max macors, we use std::min / std::max instead
#define NOMINMAX

// ATL includes
#include <atlbase.h>
#include <atlstr.h>
#define _WTL_NO_CSTRING // don't use WTL CString
#include <atltypes.h>
#include <atlwin.h>
#include <atlcom.h>

// WTL includes
#include <atlapp.h>
#include <atlgdi.h>

// Win32 includes
#include <shellapi.h>

// simulate atlmisc.h for atlctxmenu.h
#define __ATLMISC_H__

// shell extension includes
#include "atlctxmenu.h"
#include "atlshellex.h"
