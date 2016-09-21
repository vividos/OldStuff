//
// Emulator - Simple C64 emulator
// Copyright (C) 2003-2016 Michael Fink
//
/// \file Common.hpp Commonly used functions
//
#pragma once

// includes

/// creates a 16-bit word from two bytes, in little endian order
WORD LE2WORD(BYTE bLow, BYTE bHigh);

/// returns bit from value
inline bool GetBit(BYTE value, BYTE numBit)
{
   return (value & (1 << numBit)) != 0;
}
