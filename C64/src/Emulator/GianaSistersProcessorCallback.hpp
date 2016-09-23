//
// c64emu - C64 Emulator using SDL
// Copyright (C) 2006-2016 Michael Fink
//
/// \file GianaSistersProcessorCallback.hpp Processor callback for Giana Sisters game
//
#pragma once

// includes
#include "Machine.hpp"

/// Processor callback for Giana Sisters game
class GianaSistersProcessorCallback : public C64::IProcessorCallback
{
public:
   /// ctor
   GianaSistersProcessorCallback(C64::Machine& emulator)
      :m_emulator(emulator)
   {
#ifdef _DEBUG
      m_emulator.GetVideoInterfaceController().SetShowDebugInfo(true);
#endif
   }

   /// called when program counter has changed
   virtual void OnProgramCounterChange() override
   {
      C64::Processor6510& proc = m_emulator.GetProcessor();
      WORD wPC = proc.GetProgramCounter();

      if (wPC >= 0xa000 && wPC < 0xc000 &&
         (m_emulator.GetMemoryManager().Peek(1) & C64::maskBasicROM) != 0)
      {
         switch (wPC)
         {
         default:
            ATLTRACE(_T("PC change to basic ROM, $%04x\n"), wPC);
            break;
         }
      }

      if (wPC >= 0xe000 &&
         (m_emulator.GetMemoryManager().Peek(1) & C64::maskKernalROM) != 0)
      {
         switch (wPC)
         {
         case 0xFDA3: // initialize interrupt
            //m_emulator.GetProcessor().SetFlag(C64::flagInterrupt, false); // CLI
            //m_emulator.GetProcessor().Return();
            break;

         case 0xFD15: // init hardware & IO vectors
         case 0xFF5B: // video reset
         case 0xFFD2: // BSOUT
            m_emulator.GetProcessor().Return();
            break;

         case 0xFF48: // IRQ
            proc.Push(proc.GetRegister(C64::regA));
            proc.Push(proc.GetRegister(C64::regX));
            proc.Push(proc.GetRegister(C64::regY));
            proc.SetProgramCounter(m_emulator.GetMemoryManager().Peek16(0x0314));
            break;

         default:
            ATLTRACE(_T("PC change to kernal ROM, $%04x\n"), wPC);
            m_emulator.GetProcessor().Return();
            break;
         }
      }
   }

   /// called when processor made a step
   virtual void OnStep() override
   {
      WORD wPC = m_emulator.GetProcessor().GetProgramCounter();

      if (wPC == 0x1000)
      {
         m_emulator.GetProcessor().SetProgramCounter(0x161C);
         m_emulator.GetMemoryManager().Poke(0xFFFE, 0x48);
         m_emulator.GetMemoryManager().Poke(0xFFFF, 0xFF);
      }

      if (wPC == 0x015D)
         m_emulator.GetProcessor().SetDebugOutput(true);

      if (wPC == 0x0000)
      {
         WORD interruptPC = m_emulator.GetMemoryManager().Peek16(0x0314);
         m_emulator.GetProcessor().SetProgramCounter(interruptPC);
      }
   }

private:
   /// C64 machine
   C64::Machine& m_emulator;
};
