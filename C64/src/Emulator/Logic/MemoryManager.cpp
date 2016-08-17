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
}

void MemoryManager::SetHandler(MemoryHandler enMemoryHandler, std::shared_ptr<IMemory> spIMemory)
{
   ATLASSERT(m_vecMemoryHandler.size() == handlerLast + 1);
   ATLASSERT(static_cast<size_t>(enMemoryHandler) < m_vecMemoryHandler.size());

   m_vecMemoryHandler[static_cast<size_t>(enMemoryHandler)] = spIMemory;
}

/*
   100 rem *** where am i? ***
   105 rem -------------------------------
   110 rem this is the recommended method
   115 rem how to detect a c64 emulator,
   120 rem e.g. for disabling fast loaders
   125 rem
   130 rem - the byte at $dfff changes
   135 rem   between $55 and $aa
   140 rem - the byte at $dffe contains
   145 rem   the manufacturer code letter:
   150 rem   a = c64alive
   152 rem   f = frodo
   155 rem   l = mc64 beta 10-12, now free
   160 rem   m = mc64 beta 13-17, now free
   161 rem   p = personal c64
   175 rem   s = c64 software emulator
   180 rem   x = x64
   185 rem - the word at $dffc contains
   190 rem   the emulator version number,
   195 rem   e.g. $0120 for version 1.2
   200 rem - the bytes from $dfa0 contain
   205 rem   a copyright string with
   210 rem   emulator name and version,
   215 rem   $0d, copyright and $00. this
   220 rem   was at $df00 in early mc64
   225 rem   betas, but had to be moved
   230 rem   because $df00 is reserved for
   235 rem   ram expansion units
*/

BYTE MemoryManager::Peek(WORD wAddr) const
{
   ATLASSERT(m_vecMemoryHandler.size() == handlerLast + 1);

   // shortcut: must be in $0000-$9fff or $c000-$cfff, or mask is clear
   BYTE bBank = static_cast<BYTE>(wAddr >> 12);
   BYTE bMask = m_vecReadWriteMemory[1];
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

/*
   // note: poke always goes to the RAM, except when IO area is switched on
   // shortcut: must be in $0000-$9fff or $c000-$cfff, or mask is clear
   if (bBank != 0x0d || (bMask & maskIO) == 0)
   {
      m_vecReadWriteMemory[wAddr] = bValue;
      return;
   }
*/

   if (bBank < 0x0a || bBank == 0x0c || (bMask & maskAll) == 0)
   {
      m_vecReadWriteMemory[wAddr] = bValue;
      return;
   }

   std::shared_ptr<IMemory> spHandler = FindHandler(wAddr);

   if (spHandler.get() != NULL)
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
