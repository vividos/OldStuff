//
// Emulator - Simple C64 emulator
// Copyright (C) 2003-2016 Michael Fink
//
/// \file PETASCIIString.hpp PETASCII string class
//
#pragma once

// includes
#include <string>
#include <vector>

namespace C64
{

/// PETASCII string
class PETASCIIString
{
public:
   /// ctor
   PETASCIIString()
   {
   }
   /// dtor
   ~PETASCIIString()
   {
   }

   /// assigns memory area to PETASCII string
   void Assign(BYTE* pbData, WORD wLength){ m_vecString.assign(pbData, pbData + wLength); }

   /// adds byte to PETASCII string
   PETASCIIString& operator+=(BYTE bChar)
   {
      m_vecString.push_back(bChar);
      return *this;
   }

   /// converts PETASCII string to ANSI string
   std::string ToString() const;

private:
   /// remaps PETASCII string to screen code
   static char RemapPETASCIIToScreenCode(BYTE c);

private:
   /// PETASCII string
   std::vector<BYTE> m_vecString;
};

} // namespace C64
