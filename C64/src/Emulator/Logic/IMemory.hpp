//
// Emulator - Simple C64 emulator
// Copyright (C) 2003-2016 Michael Fink
//
/// \file IMemory.hpp Memory access interface
//
#pragma once

// includes

namespace C64
{

/// Interface for memory access
class IMemory
{
public:
   /// dtor
   virtual ~IMemory() throw() {}

   /// reads byte from address
   virtual BYTE Peek(WORD wAddr) throw() = 0;

   /// writes byte to address
   virtual void Poke(WORD wAddr, BYTE bValue) throw() = 0;
};

} // namespace C64
