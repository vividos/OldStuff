//
// PocketTextElite - port of Elite[TM] trading system
// Copyright (C) 2008-2009 Michael Fink
//
/// \file stdio.h fake stdio.h header
//
#pragma once

#ifndef APSTUDIO_READONLY_SYMBOLS

#ifdef _WIN32_WCE
   #if _WIN32_WCE < 0x500
   #include <../ARMv4/stdio.h>
   #elif _WIN32_WCE >= 0x500
   #include <../ARMV4I/stdio.h>
   #endif
#else
   #include <stdio.h>
#endif

#define printf textelite_printf
#define tolower textelite_tolower
#define toupper textelite_toupper

#ifdef __cplusplus
extern "C" {
#endif

int __cdecl textelite_printf(const char*, ...);

#ifdef __cplusplus
}
#endif

#endif // APSTUDIO_READONLY_SYMBOLS
