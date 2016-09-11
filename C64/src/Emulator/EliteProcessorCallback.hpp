//
// c64emu - C64 Emulator using SDL
// Copyright (C) 2006-2016 Michael Fink
//
/// \file EliteProcessorCallback.hpp Processor callback for Elite game
//
#pragma once

// includes
#include "Machine.hpp"

/// Processor callback for Elite game
class EliteProcessorCallback : public C64::IProcessorCallback
{
public:
   /// ctor
   EliteProcessorCallback(C64::Machine& emulator)
      :m_emulator(emulator)
   {
   }

   /// called when program counter has changed
   virtual void OnProgramCounterChange() override
   {
      WORD wPC = m_emulator.GetProcessor().GetProgramCounter();

      if (wPC >= 0xa000 && wPC < 0xc000 &&
         (m_emulator.GetMemoryManager().Peek(1) & C64::maskBasicROM) != 0)
      {
         switch (wPC)
         {
         case 0xa8bc:
            // jump to ROM routine GOTO
            m_emulator.GetProcessor().SetProgramCounter(2064);
            // simulate keypress
            //GetMemoryManager().Poke(0x00c6, 1);
            break;

         default:
            //TODO ATLTRACE(_T("PC change to basic ROM, $%04x\n"), wPC);
            break;
         }
      }

      if (wPC >= 0xe000 &&
         (m_emulator.GetMemoryManager().Peek(1) & C64::maskKernalROM) != 0)
      {
         switch (wPC)
         {
         case 0xffd2: // BSOUT
            m_emulator.GetProcessor().Return();
            break;

         default:
            ATLTRACE(_T("PC change to kernal ROM, $%04x\n"), wPC);
            break;
         }
      }
   }

   /// called when processor made a step
   virtual void OnStep() override
   {
      if (m_emulator.GetProcessor().GetProgramCounter() == 0x0844)
      {
         m_emulator.GetMemoryManager().Poke(0x00c6, 1);
      }
   }

private:
   /// C64 machine
   C64::Machine& m_emulator;
};
