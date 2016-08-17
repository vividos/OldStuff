//
// Emulator - Simple C64 emulator
// Copyright (C) 2003-2016 Michael Fink
//
/// \file VICMemoryAdapter.cpp Memory adapter for VideoInterfaceController
//

// includes
#include "StdAfx.h"
#include "VICMemoryAdapter.hpp"
#include "VideoInterfaceController.hpp"

using C64::VICMemoryAdapter;

BYTE VICMemoryAdapter::Peek(WORD wAddr) throw()
{
   if (wAddr >= 0xd000 && wAddr < 0xd400)
   {
      wAddr &= 0x3f;
      return wAddr < 47 ? m_vic.ReadRegister(static_cast<BYTE>(wAddr)) : 0xff;
   }
   else
   if (wAddr >= 0xd800 && wAddr < 0xdc00)
   {
      return m_vic.ReadColorRam(wAddr & 0x03ff);
   }
   else
      ATLASSERT(false);

   return 0;
}

void VICMemoryAdapter::Poke(WORD wAddr, BYTE bValue) throw()
{
   if (wAddr >= 0xd000 && wAddr < 0xd400)
   {
      wAddr &= 0x3f;
      if (wAddr < 47)
         m_vic.SetRegister(static_cast<BYTE>(wAddr), bValue);
   }
   else
   if (wAddr >= 0xd800 && wAddr < 0xdc00)
   {
      m_vic.SetColorRam(wAddr & 0x03ff, bValue);
   }
   else
      ATLASSERT(false);
}
