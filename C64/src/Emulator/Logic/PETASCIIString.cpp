//
// Emulator - Simple C64 emulator
// Copyright (C) 2003-2016 Michael Fink
//
/// \file PETASCIIString.cpp PETASCII string class
//

// includes
#include "StdAfx.h"
#include "PETASCIIString.hpp"

using C64::PETASCIIString;

std::string PETASCIIString::ToString() const
{
   std::string text;

   for(size_t i = 0, iMax = m_vecString.size(); i<iMax; i++)
   {
      BYTE bChar = m_vecString[i];

      RemapPETASCIIToScreenCode(bChar);
      // TODO actual conversion
//      if (bChar < 0x20 && bChar != 0x0d)
//         bChar = '?';

      text += static_cast<char>(bChar);
   }

   return text;
}

/// \details Remaps ascii characters to c64 string codes.
/// The function does the following mapping:
/// <pre>
///    screen code       ASCII code        further codes     part in code
///    0x00 - 0x20       0x40 - 0x60                         (1)
///    0x20 - 0x40       0x20 - 0x40                         n/a (no remap)
///    0x40 - 0x60       0x60 - 0x80                         (3)
///    0x60 - 0x80       0xA0 - 0xC0                         (4)
/// 
///    0x40 - 0x60       0x60 - 0x80       0xC0 - 0xE0       (5)
///    0x60 - 0x7E       0xA0 - 0xBE       0xE0 - 0xFE       (6)
///    0x5E              0x7E              0xFF              (7)
/// 
///    n/a               0x00 - 0x20
///    n/a               0x80 - 0xa0
/// </pre>
char PETASCIIString::RemapPETASCIIToScreenCode(BYTE c)
{
   // remap
   if ( 0xc0 <= c && c < 0xe0 ) c -= 0x60;      // (5)
   if ( 0xe0 <= c && c < 0xff ) c -= 0x20;      // (6)
   if ( c == 0xff ) c = 0x7e;                   // (7)

   if ( 0x40 <= c && c < 0x60 ) c -= 0x40; else // (1)
   if ( 0x60 <= c && c < 0x80 ) c -= 0x20; else // (3)
   if ( 0xa0 <= c && c < 0xc0 ) c -= 0x40;      // (4)

   return c;
}
