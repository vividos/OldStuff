//
// Emulator - Simple C64 emulator
// Copyright (C) 2003-2016 Michael Fink
//
/// \file SoundInterfaceDevice.cpp Sound Interface Device
//

// includes
#include "stdafx.h"
#include "SoundInterfaceDevice.hpp"

using C64::SoundInterfaceDevice;

void SoundInterfaceDevice::SetRegister(BYTE bRegister, BYTE bValue)
{
   bRegister;
   bValue;
}

BYTE SoundInterfaceDevice::ReadRegister(BYTE bRegister)
{
   bRegister;
   return 0xFF;
}
