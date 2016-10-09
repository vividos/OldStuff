//
// ulib - a collection of useful classes
// Copyright (C) 2013-2016 Michael Fink
//
/// \file Android.hpp Android NDK config
//
#pragma once

// includes
#include <cstdint>
#include <cstdarg>
#include <cassert>
#include <android/log.h>

// override is unknown on NDK
#define override

// Win32 types
typedef uint_least32_t UINT;
typedef uint_least8_t BYTE;
typedef uint_least16_t WORD;
typedef uint_least32_t DWORD;
typedef unsigned long long ULONGLONG;
typedef signed long long LONGLONG;

typedef char CHAR;
typedef CHAR* LPSTR;
typedef const CHAR* LPCSTR;
typedef wchar_t WCHAR;
typedef WCHAR* LPWSTR;
typedef const WCHAR* LPCWSTR;
typedef CHAR TCHAR;
typedef const TCHAR* LPCTSTR;

typedef TCHAR _TCHAR;

#define _T(x) x

// android logging

inline void AndroidLog(android_LogPriority prio, LPCSTR tag, LPCSTR file, UINT line, LPCSTR format, ...)
{
   va_list args;
   va_start(args, format);
   __android_log_vprint(prio, tag, format, args);
}

// trick to make declspec expand to nothing on android
#define __declspec(xxx)

// ATL stuff
#define ATLASSERT assert
#define ATLTRACE(...) AndroidLog(ANDROID_LOG_DEBUG, "", __FILE__, __LINE__, __VA_ARGS__)
#ifdef _DEBUG
#define ATLVERIFY(x) ATLASSERT(x)
#else
#define ATLVERIFY(x) x
#endif

// tchar.h stuff
#define _tfopen fopen
#define _ftprintf fprintf

#define __nop()
