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
