//
// Emulator - Simple C64 emulator
// Copyright (C) 2003-2016 Michael Fink
//
/// \file TestEmulator.hpp Emulator implementation for unit tests
//
#pragma once

// includes
#include "Machine.hpp"
#include "PC64File.hpp"
#include "PETASCIIString.hpp"

/// Emulator for unit tests
class TestEmulator :
   public C64::Machine,
   public C64::IProcessorCallback
{
public:
   /// ctor
   TestEmulator()
      :m_bExit(false),
      m_bTestResult(false)
   {
      GetProcessor().AddProcessorCallback(this);
   }
   /// dtor
   ~TestEmulator()
   {
      GetProcessor().RemoveProcessorCallback(this);
   }

   /// Loads test from path, with filename
   bool LoadTest(LPCTSTR pszTestPath, LPCTSTR pszTestName)
   {
      CString cszFilename;
      cszFilename.Format(_T("%s\\%s.p00"), pszTestPath, pszTestName);

      C64::PC64File file(cszFilename);
      if (!file.IsValid())
         return false;

      return file.Load(GetMemoryManager().GetRAM());
   }

   /// Runs test and checks if test executes OK
   bool Run()
   {
      C64::Processor6510& proc = GetProcessor();

      proc.SetDebugOutput(true);

      // set IRQ handler
      GetMemoryManager().Poke(0xfffe, 0x48);
      GetMemoryManager().Poke(0xffff, 0xff);

      // set stack pointer to a reasonable value
      proc.SetRegister(C64::regSP, 0xff);

      m_bExit = m_bTestResult = false;
      for (; !m_bExit;)
      {
         // check if program wants to exit per RTS
         /*
         if (proc.GetRegister(C64::regSP) == 0xff &&
         GetMemoryManager().Peek(proc.GetProgramCounter()) == C64::opRTS)
         {
         m_bExit = true;
         break;
         }
         */
         proc.Step();
      }

      // create output string
      std::string output = m_strOutput.ToString();

      ATLTRACE(_T("%hs\n"), output.c_str());

      // search output for string " - OK"
      m_bTestResult = std::string::npos != output.find("- OK");

      return m_bTestResult;
   }

private:
   /// called when program counter has changed
   virtual void OnProgramCounterChange()
   {
      C64::Processor6510& proc = GetProcessor();
      C64::MemoryManager& mem = GetMemoryManager();
      WORD wPC = proc.GetProgramCounter();

      switch (wPC)
      {
      case 0x0000: // indirect jump to $0000
      case 0xe16f: // LOAD command
         m_bExit = true;
         break;

      case 0xff48:
      {
         ATLTRACE(_T("KERNAL: IRQ Handler\n"));

         proc.Push(proc.GetRegister(C64::regA));               // save A
         proc.SetRegister(C64::regA, proc.GetRegister(C64::regX));  // save X
         proc.Push(proc.GetRegister(C64::regA));
         proc.SetRegister(C64::regA, proc.GetRegister(C64::regY));  // save Y
         proc.Push(proc.GetRegister(C64::regA));
         proc.SetRegister(C64::regX, proc.GetRegister(C64::regSP)); // get stack pointer

         // load status register from stack
         BYTE bSR = mem.Peek(0x0100 + ((proc.GetRegister(C64::regX) + 4) & 0xff));

         // decide which type of interrupt
         WORD wAddr = (bSR & 0x10) == 0 ? 0x0314 : 0x0316;
         proc.SetProgramCounter(mem.Peek16(wAddr));
      }
      break;

      case 0xffd2: // BSOUT
      {
         BYTE bRegA = proc.GetRegister(C64::regA);
         if (bRegA != 0)
            m_strOutput += bRegA;
         proc.Return();

         //ATLTRACE(_T("KERNAL: BSOUT: %c #$%02x\n"), bRegA < 0x20 ? _T('?') : bRegA, bRegA);
      }
      break;

      case 0xffe4: // GETIN
         ATLTRACE(_T("KERNAL: GETIN\n"));
         proc.SetRegister(C64::regA, 0x3); // simulate keypress with code 3
         proc.Return();
         break;

      default:
         //ATLTRACE("test emulator changed PC to $%04x\n", wPC);
         break;
      }
   }

private:
   /// set to true when test can exit
   bool m_bExit;

   /// test result
   bool m_bTestResult;

   /// output as PETASCII string
   C64::PETASCIIString m_strOutput;
};
