//
// Emulator - Simple C64 emulator
// Copyright (C) 2003-2016 Michael Fink
//
/// \file SIDMemoryAdapter.cpp Memory adapter for SoundInterfaceDevice
//

// includes
#include "stdafx.h"
#include "SIDMemoryAdapter.hpp"
#include "SoundInterfaceDevice.hpp"

using C64::SIDMemoryAdapter;

BYTE SIDMemoryAdapter::Peek(WORD wAddr) throw()
{
   if (wAddr < 0xd400 || wAddr >= 0xd418)
      return 0xFF;

   ATLTRACE(_T("SID register read at $%04x\n"), wAddr);

   return m_sid.ReadRegister(static_cast<BYTE>(wAddr - 0xd400));
}

void SIDMemoryAdapter::Poke(WORD wAddr, BYTE bValue) throw()
{
   if (wAddr < 0xd400 || wAddr >= 0xd418)
      return;

   ATLTRACE(_T("SID register write at $%04x = $%02x\n"), wAddr, bValue);

   m_sid.SetRegister(static_cast<BYTE>(wAddr - 0xd400), bValue);
}
