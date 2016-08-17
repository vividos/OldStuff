//
// Emulator - Simple C64 emulator
// Copyright (C) 2003-2016 Michael Fink
//
/// \file CIAMemoryAdapter.hpp Memory adapter for ComplexInterfaceAdapter
//
#pragma once

// includes
#include "IMemory.hpp"

namespace C64
{
// forward references
class ComplexInterfaceAdapter;

/// Memory adapter for ComplexInterfaceAdapter
class CIAMemoryAdapter : public IMemory
{
public:
   /// ctor; initializes memory adapter, using a CIA base address
   CIAMemoryAdapter(ComplexInterfaceAdapter& cia, WORD wBaseAddr)
      :m_cia(cia),
       m_wBaseAddr(wBaseAddr)
   {
   }

   /// dtor
   virtual ~CIAMemoryAdapter() throw()
   {
   }

private:
   // virtual methods from IMemory

   /// reads byte from address
   virtual BYTE Peek(WORD wAddr) throw() override;

   /// writes byte to address
   virtual void Poke(WORD wAddr, BYTE bValue) throw() override;

private:
   /// CIA to be used for reading/writing
   ComplexInterfaceAdapter& m_cia;

   /// memory base address for CIA
   WORD m_wBaseAddr;
};

} // namespace C64
