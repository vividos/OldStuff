//
// Emulator - Simple C64 emulator
// Copyright (C) 2003-2016 Michael Fink
//
/// \file Machine.cpp C64 machine class
//

// includes
#include "StdAfx.h"
#include "Machine.hpp"
#include "VICMemoryAdapter.hpp"
#include "SIDMemoryAdapter.hpp"
#include "CIAMemoryAdapter.hpp"

using C64::Machine;

Machine::Machine()
:m_processor(m_memoryManager),
 m_vic(m_memoryManager, m_processor),
 m_sid(m_memoryManager)
{
   m_memoryManager.SetHandler(handlerVIC, std::shared_ptr<IMemory>(new VICMemoryAdapter(m_vic)));
   m_memoryManager.SetHandler(handlerCIA1, std::shared_ptr<IMemory>(new CIAMemoryAdapter(m_cia1, 0xdc00)));
   m_memoryManager.SetHandler(handlerCIA2, std::shared_ptr<IMemory>(new CIAMemoryAdapter(m_cia2, 0xdd00)));
   m_memoryManager.SetHandler(handlerSID, std::shared_ptr<IMemory>(new SIDMemoryAdapter(m_sid)));

   m_cia1.SetPortListener(&m_keyboard);
   m_cia2.SetPortListener(&m_vic);
}

Machine::~Machine() throw()
{
}

void Machine::Run()
{
   for(;;)
   {
      m_vic.Step();
      m_processor.Step();
   }
}
