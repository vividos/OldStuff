//
// c64emu - C64 Emulator using SDL
// Copyright (C) 2006-2016 Michael Fink
//
/// \file ProcessorPerformanceMap.hpp Processor performance map
//
#pragma once

// includes
#include "Processor6510.hpp"
#include <array>

/// processor performance map; collects loads, stores and opcode bytes
class ProcessorPerformanceMap : public C64::IProcessorCallback
{
public:
   /// ctor
   ProcessorPerformanceMap();

   /// clears performance map
   void Clear();

   /// returns performance map; non-const version
   Uint8* Data() { return m_performanceMap.data(); }

private:
   // virtual methods from IProcessorCallback

   /// called when a memory load is made
   virtual void OnLoad(WORD address) override;

   /// called when a memory store is made
   virtual void OnStore(WORD address) override;

   /// called when an opcode is executed
   virtual void OnExecute(WORD address, unsigned int instructionSize) override;

private:
   /// performance map for all 64k memory
   std::array<Uint8, 256 * 256> m_performanceMap;
};
