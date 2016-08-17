//
// Emulator - Simple C64 emulator
// Copyright (C) 2003-2016 Michael Fink
//
/// \file VICMemoryAdapter.hpp Memory adapter for VideoInterfaceController
//
#pragma once

// includes
#include "IMemory.hpp"

namespace C64
{
// forward references
class VideoInterfaceController;

/// Memory adapter for VideoInterfaceController
class VICMemoryAdapter : public IMemory
{
public:
   /// ctor
   VICMemoryAdapter(VideoInterfaceController& vic)
      :m_vic(vic)
   {
   }

   /// dtor
   virtual ~VICMemoryAdapter() throw()
   {
   }

private:
   // virtual methods from IMemory

   /// reads byte from address
   virtual BYTE Peek(WORD wAddr) throw() override;

   /// writes byte to address
   virtual void Poke(WORD wAddr, BYTE bValue) throw() override;

private:
   /// reference to VIC
   VideoInterfaceController& m_vic;
};

} // namespace C64
