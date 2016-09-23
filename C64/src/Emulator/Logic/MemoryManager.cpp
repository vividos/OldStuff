//
// Emulator - Simple C64 emulator
// Copyright (C) 2003-2016 Michael Fink
//
/// \file MemoryManager.cpp Memory manager implementation
//

// includes
#include "StdAfx.h"
#include "MemoryManager.hpp"
#include "IMemory.hpp"

using C64::MemoryManager;
using C64::MemoryHandler;
using C64::IMemory;

MemoryManager::MemoryManager()
:m_vecReadWriteMemory(64*1024), // 64k memory
 m_vecMemoryHandler(handlerLast+1)
{
   // default: 0x37, or 0011 0111
   // bit 0: basic ROM/RAM (1/0)
   // bit 1: kernal ROM/RAM (1/0)
   // bit 2: IO area/character rom (1/0)
   m_vecReadWriteMemory[1] = 0x37;
}

void MemoryManager::SetHandler(MemoryHandler enMemoryHandler, std::shared_ptr<IMemory> spIMemory)
{
   ATLASSERT(m_vecMemoryHandler.size() == handlerLast + 1);
   ATLASSERT(static_cast<size_t>(enMemoryHandler) < m_vecMemoryHandler.size());

   m_vecMemoryHandler[static_cast<size_t>(enMemoryHandler)] = spIMemory;
}

BYTE MemoryManager::Peek(WORD wAddr) const
{
   ATLASSERT(m_vecMemoryHandler.size() == handlerLast + 1);

   BYTE bBank = static_cast<BYTE>(wAddr >> 12);
   BYTE bMask = m_vecReadWriteMemory[1];

   // shortcut: must be in $0000-$9fff or $c000-$cfff, or mask is clear
   if (bBank < 0x0a || bBank == 0x0c || (bMask & maskAll) == 0)
      return m_vecReadWriteMemory[wAddr];

   std::shared_ptr<IMemory> spHandler = FindHandler(wAddr);

   if (spHandler.get() != NULL)
      return spHandler->Peek(wAddr);
   else
      return m_vecReadWriteMemory[wAddr];
}

void MemoryManager::Poke(WORD wAddr, BYTE bValue)
{
   BYTE bBank = static_cast<BYTE>(wAddr >> 12); // leftmost nibble
   BYTE bMask = m_vecReadWriteMemory[0x01];

   // note: poke always goes to the RAM, except when IO area is switched on
   // shortcut: must be in $0000-$9fff or $c000-$cfff, or mask is clear
   if (bBank != 0x0d || (bMask & maskIO) == 0)
   {
      m_vecReadWriteMemory[wAddr] = bValue;
      return;
   }

   std::shared_ptr<IMemory> spHandler = FindHandler(wAddr);

   if (spHandler != nullptr)
      spHandler->Poke(wAddr, bValue);
   else
      m_vecReadWriteMemory[wAddr] = bValue;
}

std::shared_ptr<IMemory> MemoryManager::FindHandler(WORD wAddr) const
{
   BYTE bBank = static_cast<BYTE>(wAddr >> 12);
   BYTE bMask = m_vecReadWriteMemory[1];

   // basic ROM?
   if ((bBank == 0x0a || bBank == 0x0b) && (bMask & maskBasicROM) != 0)
      return m_vecMemoryHandler[handlerBasicROM];

   // kernal ROM?
   if ((bBank == 0x0e || bBank == 0x0f) && (bMask & maskKernalROM) != 0)
      return m_vecMemoryHandler[handlerKernalROM];

   // now it must be $dxxx
   if ((wAddr >= 0xd000 && wAddr < 0xd400) || (wAddr >= 0xd800 && wAddr < 0xdc00))
      return m_vecMemoryHandler[handlerVIC];
   else if (wAddr >= 0xd400 && wAddr < 0xd800)
      return m_vecMemoryHandler[handlerSID];
   else if (wAddr >= 0xdc00 && wAddr < 0xdd00)
      return m_vecMemoryHandler[handlerCIA1];
   else if (wAddr >= 0xdd00 && wAddr < 0xde00)
      return m_vecMemoryHandler[handlerCIA2];

   return std::shared_ptr<IMemory>();
}
