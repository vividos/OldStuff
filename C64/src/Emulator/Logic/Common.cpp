//
// Emulator - Simple C64 emulator
// Copyright (C) 2003-2016 Michael Fink
//
/// \file Common.cpp Commonly used functions
//
#pragma once

// includes
#include "StdAfx.h"
#include "Common.hpp"

WORD LE2WORD(BYTE bLow, BYTE bHigh)
{
   return static_cast<WORD>(bLow) | static_cast<WORD>(static_cast<WORD>(bHigh) << 8);
}
