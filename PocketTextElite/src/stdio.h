//
// PocketTextElite - port of Elite[TM] trading system
// Copyright (C) 2008-2009 Michael Fink
//
/// \file stdio.h fake stdio.h header
//
#pragma once

#ifndef APSTUDIO_READONLY_SYMBOLS

#ifdef UNDER_CE
#include <../ARMv4/stdio.h>
#else
#include <stdio.h>
#endif

#define printf textelite_printf
#define tolower textelite_tolower

#ifdef __cplusplus
extern "C" {
#endif

int __cdecl textelite_printf(const char*, ...);

#ifdef __cplusplus
}
#endif

#endif // APSTUDIO_READONLY_SYMBOLS
