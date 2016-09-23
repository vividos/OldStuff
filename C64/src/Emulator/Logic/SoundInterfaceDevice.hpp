//
// Emulator - Simple C64 emulator
// Copyright (C) 2003-2016 Michael Fink
//
/// \file SoundInterfaceDevice.cpp Sound Interface Device
//
#pragma once

// includes

namespace C64
{
// forward references
class MemoryManager;

/// \brief SID implementation
class SoundInterfaceDevice
{
public:
   SoundInterfaceDevice(MemoryManager& memoryManager)
   {
      memoryManager;
   }

   ~SoundInterfaceDevice()
   {
   }

   /// sets a register value
   void SetRegister(BYTE bRegister, BYTE bValue);

   /// reads register value
   BYTE ReadRegister(BYTE bRegister);
};

} // namespace C64
