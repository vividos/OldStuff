//
// c64emu - C64 Emulator using SDL
// Copyright (C) 2006-2016 Michael Fink
//
/// \file ProcessorPerformanceMap.cpp Processor performance map
//

// includes
#include "stdafx.h"
#include "ProcessorPerformanceMap.hpp"

ProcessorPerformanceMap::ProcessorPerformanceMap()
{
   Clear();
}

void ProcessorPerformanceMap::Clear()
{
   std::fill_n(m_performanceMap.begin(), m_performanceMap.size(), 0); // black
}

void ProcessorPerformanceMap::OnLoad(WORD address)
{
   ATLASSERT(address < m_performanceMap.size());

   m_performanceMap[address] = 5; // green
}

void ProcessorPerformanceMap::OnStore(WORD address)
{
   ATLASSERT(address < m_performanceMap.size());

   m_performanceMap[address] = 2; // red
}

void ProcessorPerformanceMap::OnExecute(WORD address, unsigned int instructionSize)
{
   ATLASSERT(address < m_performanceMap.size());

   std::fill_n(m_performanceMap.begin() + address, instructionSize, 4); // pink
}
