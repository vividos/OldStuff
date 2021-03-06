//
// Emulator - Simple C64 emulator
// Copyright (C) 2003-2016 Michael Fink
//
/// \file Machine.hpp C64 machine class
//
#pragma once

// includes
#include "MemoryManager.hpp"
#include "Processor6510.hpp"
#include "VideoInterfaceController.hpp"
#include "SoundInterfaceDevice.hpp"
#include "ComplexInterfaceAdapter.hpp"
#include "Keyboard.hpp"

namespace C64
{

/// C64 machine
class Machine
{
public:
   /// ctor; initializes all objects
   Machine();

   /// dtor
   ~Machine() throw();

   /// returns memory manager
   MemoryManager& GetMemoryManager() throw() { return m_memoryManager; }

   /// returns processor
   Processor6510& GetProcessor() throw() { return m_processor; }

   /// returns VIC
   VideoInterfaceController& GetVideoInterfaceController() throw() { return m_vic; }

   /// returns SID
   SoundInterfaceDevice& GetSoundInterfaceDevice() throw() { return m_sid; }

   /// returns keyboard
   Keyboard& GetKeyboard() throw() { return m_keyboard; }

   /// runs emulator indefinitely
   void Run();

private:
   MemoryManager m_memoryManager;   ///< memory manager
   Processor6510 m_processor;       ///< processor
   VideoInterfaceController m_vic;  ///< VIC chip
   SoundInterfaceDevice m_sid;      ///< SID chip
   ComplexInterfaceAdapter m_cia1;  ///< CIA chip 1
   ComplexInterfaceAdapter m_cia2;  ///< CIA chip 2
   Keyboard m_keyboard;             ///< keyboard
};

} // namespace C64
