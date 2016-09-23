//
// Emulator - Simple C64 emulator
// Copyright (C) 2003-2016 Michael Fink
//
/// \file SIDMemoryAdapter.hpp Memory adapter for SoundInterfaceDevice
//
#pragma once

// includes
#include "IMemory.hpp"

namespace C64
{
// forward references
class SoundInterfaceDevice;

/// Memory adapter for SoundInterfaceDevice
class SIDMemoryAdapter : public IMemory
{
public:
   /// ctor; initializes memory adapter
   SIDMemoryAdapter(SoundInterfaceDevice& sid)
      :m_sid(sid)
   {
   }

   /// dtor
   virtual ~SIDMemoryAdapter() throw()
   {
   }

private:
   // virtual methods from IMemory

   /// reads byte from address
   virtual BYTE Peek(WORD wAddr) throw() override;

   /// writes byte to address
   virtual void Poke(WORD wAddr, BYTE bValue) throw() override;

private:
   /// SID to be used for reading/writing
   SoundInterfaceDevice& m_sid;
};

} // namespace C64
