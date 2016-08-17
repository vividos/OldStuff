//
// Emulator - Simple C64 emulator
// Copyright (C) 2003-2016 Michael Fink
//
/// \file MemoryManager.hpp Memory manager implementation
//
#pragma once

// includes
#include <vector>
#include <memory>
#include "Common.hpp"

namespace C64
{
// forward references
class IMemory;

/// Memory handler type
enum MemoryHandler
{
   handlerBasicROM = 0,
   handlerKernalROM,
   handlerVIC,
   handlerSID,       ///< SID area $d400-$d7ff
   handlerCIA1,      ///< CIA 1 handler $dc00-$dc0f
   handlerCIA2,      ///< CIA 2 handler $dd00-$dd0f

   handlerLast = handlerCIA2,
};

/// Port mask to select different memory banks in register $01
enum PortMask
{
   maskBasicROM = 1,    ///< when set in $01, the basic rom is visible to the processor
   maskKernalROM = 2,   ///< when set in $01, the kernal rom is visible
   maskIO = 4,          ///< when set in $01, the IO area is mapped in $D000, else the charset is
   maskAll = 7,         ///< mask for all port mask flags
   maskRemove = 0xf8,   ///< mask to remove all bits in port mask
};

/// Memory manager for C64 memory banks
class MemoryManager
{
public:
   /// ctor
   MemoryManager();
   /// dtor
   ~MemoryManager() throw()
   {
   }

   /// returns RAM memory
   std::vector<BYTE>& GetRAM(){ return m_vecReadWriteMemory; }
   /// returns RAM memory; const version
   const std::vector<BYTE>& GetRAM() const { return m_vecReadWriteMemory; }

   /// sets handler for given memory handler type
   void SetHandler(MemoryHandler enMemoryHandler, std::shared_ptr<IMemory> spIMemory);

   /// reads a byte from given memory address
   BYTE Peek(WORD wAddr) const;

   /// reads a 16-bit word from given memory address
   WORD Peek16(WORD wAddr) const
   {
      return LE2WORD(Peek(wAddr), Peek(wAddr+1));
   }

   /// writes a byte to given memory address
   void Poke(WORD wAddr, BYTE bValue);

private:
   /// finds a handler for a given address, if available (may return nullptr)
   std::shared_ptr<IMemory> FindHandler(WORD wAddr) const;

private:
   /// RAM memory bank
   std::vector<BYTE> m_vecReadWriteMemory;

   /// all memory handler
   std::vector<std::shared_ptr<IMemory>> m_vecMemoryHandler;
};

} // namespace C64
