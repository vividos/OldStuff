//
// ulib - a collection of useful classes
// Copyright (C) 2016 Michael Fink
//
/// \file Atl.hpp ATL include file
//
#pragma once

#ifdef WIN32

// Win32
#define WIN32_LEAN_AND_MEAN
#include <SDKDDKVer.h>

#include <atlbase.h>
#include <atlstr.h>

#elif defined(__ANDROID__)

// Android
#include <cstdint>
typedef uint_least8_t BYTE;
typedef uint_least16_t WORD;
typedef uint_least32_t DWORD;
#define _T(x) L##x
#define ATLTRACE (void)
#define ATLASSERT(x) ;
//#include "AndroidString.hpp"

#endif
