//
// Emulator - Simple C64 emulator
// Copyright (C) 2003-2016 Michael Fink
//
/// \file Processor6510.cpp MOS6510 processor emulation
//

// includes
#include "StdAfx.h"
#include "Processor6510.hpp"
#include "MemoryManager.hpp"

using C64::Processor6510;
using C64::AddressingMode;
using C64::RegisterID;

/// \brief cycle count for all opcodes
/// from http://www.oxyron.de/html/opcodes02.html
static BYTE g_abOpcodeCycles[256] =
{
   7, 6, 0, 8, 3, 3, 5, 5, 3, 2, 2, 2, 4, 4, 6, 6,
   2, 5, 0, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7, 
   6, 6, 0, 8, 3, 3, 5, 5, 4, 2, 2, 2, 4, 4, 6, 6, 
   2, 5, 0, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
   6, 6, 0, 8, 3, 3, 5, 5, 3, 2, 2, 2, 3, 4, 6, 6,
   2, 5, 0, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
   6, 6, 0, 8, 3, 3, 5, 5, 4, 2, 2, 2, 5, 4, 6, 6,
   2, 5, 0, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
   2, 6, 2, 6, 3, 3, 3, 3, 2, 2, 2, 2, 4, 4, 4, 4,
   2, 6, 0, 6, 4, 4, 4, 4, 2, 5, 2, 5, 5, 5, 5, 5,
   2, 6, 2, 6, 3, 3, 3, 3, 2, 2, 2, 2, 4, 4, 4, 4,
   2, 5, 0, 5, 4, 4, 4, 4, 2, 4, 2, 4, 4, 4, 4, 4,
   2, 6, 2, 8, 3, 3, 5, 5, 2, 2, 2, 2, 4, 4, 6, 6,
   2, 5, 0, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
   2, 6, 2, 8, 3, 3, 5, 5, 2, 2, 2, 2, 4, 4, 6, 6,
   2, 5, 0, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7
};

/// \brief constant with status register bits that are always set
/// note: bit 5 (unused) and bit 4 (break) of the status register are always set
const BYTE c_bStatusMaskAlwaysSet =
   (1 << static_cast<BYTE>(C64::flagUnused)) |
   (1 << static_cast<BYTE>(C64::flagBreak));

Processor6510::Processor6510(C64::MemoryManager& memoryManager) throw()
:m_memoryManager(memoryManager),
 m_wProgramCounter(0),
 m_uiProcessedOpcodes(0),
 m_uiElapsedCycles(0),
 m_debugOutput(false)
{
   memset(m_abRegister, 0, sizeof(m_abRegister));

   // note: bit 5 (unused) and bit 4 (break) are always set
   m_abRegister[regSR] = c_bStatusMaskAlwaysSet;
   m_abRegister[regSP] = 0xff;
}

void Processor6510::AddProcessorCallback(C64::IProcessorCallback* pCallback)
{
   ATLASSERT(pCallback != NULL);
   m_setCallbacks.insert(pCallback);
}

void Processor6510::RemoveProcessorCallback(C64::IProcessorCallback* pCallback)
{
   ATLASSERT(pCallback != NULL);
   m_setCallbacks.erase(pCallback);
}

void Processor6510::SetFlag(C64::ProcessorFlag enFlag, bool bFlag)
{
   ATLASSERT(enFlag < 8);

   BYTE iBit = static_cast<BYTE>(enFlag);
   m_abRegister[regSR] &= ~(1 << iBit);
   if (bFlag)
      m_abRegister[regSR] |= 1 << iBit;
}

bool Processor6510::GetFlag(C64::ProcessorFlag enFlag)
{
   ATLASSERT(enFlag < 8);

   BYTE iBit = static_cast<BYTE>(enFlag);

   return (m_abRegister[regSR] & (1 << iBit)) != 0;
}

void Processor6510::Step()
{
#if 0
   {
#pragma warning(disable: 28159) // Consider using GetTickCount64
      static DWORD dwT1 = GetTickCount();
      static unsigned int uiLastProcessedCycles = 0;
      DWORD dwT2 = GetTickCount();

      if (dwT2 - dwT1 > 1000)
      {
         // output some statistics
         ATLTRACE(_T("statistics: %u opcodes/s, %u cycles/s\n"),
            m_uiProcessedOpcodes,
            m_uiElapsedCycles - uiLastProcessedCycles);

         m_uiProcessedOpcodes = 0;
         uiLastProcessedCycles = m_uiElapsedCycles;
         dwT1 = dwT2;
      }
#pragma warning(default: 28159)
   }
#endif

   m_uiProcessedOpcodes++;

   // send notification of PC change
   {
      std::set<IProcessorCallback*>::const_iterator iter, stop;
      iter = m_setCallbacks.begin(); stop = m_setCallbacks.end();
      for(;iter != stop; iter++)
         (*iter)->OnStep();
   }

   if (m_debugOutput)
      m_opcodeText.Format(_T("%04x "), m_wProgramCounter);

   BYTE bOpcode = m_memoryManager.Peek(m_wProgramCounter++);

   // count cycles
   m_uiElapsedCycles += g_abOpcodeCycles[bOpcode];

   bool bProgramCounterChanged = false;

   switch(bOpcode)
   {
      // load opcodes
   case opLDA_imm: LoadRegister(regA, addrImmediate); break;
   case opLDA_zp:  LoadRegister(regA, addrZeropage); break;
   case opLDA_zpx: LoadRegister(regA, addrZeropageIndexedX); break;
   case opLDA_izx: LoadRegister(regA, addrIndirectZeropageIndexedX); break;
   case opLDA_izy: LoadRegister(regA, addrIndirectZeropageIndexedY); break;
   case opLDA_abs: LoadRegister(regA, addrAbsolute); break;
   case opLDA_abx: LoadRegister(regA, addrAbsoluteIndexedX); break;
   case opLDA_aby: LoadRegister(regA, addrAbsoluteIndexedY); break;

   case opLDX_imm: LoadRegister(regX, addrImmediate); break;
   case opLDX_zp:  LoadRegister(regX, addrZeropage); break;
   case opLDX_zpy: LoadRegister(regX, addrZeropageIndexedY); break;
   case opLDX_abs: LoadRegister(regX, addrAbsolute); break;
   case opLDX_aby: LoadRegister(regX, addrAbsoluteIndexedY); break;

   case opLDY_imm: LoadRegister(regY, addrImmediate); break;
   case opLDY_zp:  LoadRegister(regY, addrZeropage); break;
   case opLDY_zpx: LoadRegister(regY, addrZeropageIndexedX); break;
   case opLDY_abs: LoadRegister(regY, addrAbsolute); break;
   case opLDY_abx: LoadRegister(regY, addrAbsoluteIndexedX); break;

      // store opcodes
   case opSTA_zp:  StoreRegister(regA, addrZeropage); break;
   case opSTA_zpx: StoreRegister(regA, addrZeropageIndexedX); break;
   case opSTA_izx: StoreRegister(regA, addrIndirectZeropageIndexedX); break;
   case opSTA_izy: StoreRegister(regA, addrIndirectZeropageIndexedY); break;
   case opSTA_abs: StoreRegister(regA, addrAbsolute); break;
   case opSTA_abx: StoreRegister(regA, addrAbsoluteIndexedX); break;
   case opSTA_aby: StoreRegister(regA, addrAbsoluteIndexedY); break;

   case opSTX_zp:  StoreRegister(regX, addrZeropage); break;
   case opSTX_zpy: StoreRegister(regX, addrZeropageIndexedY); break;
   case opSTX_abs: StoreRegister(regX, addrAbsolute); break;

   case opSTY_zp:  StoreRegister(regY, addrZeropage); break;
   case opSTY_zpx: StoreRegister(regY, addrZeropageIndexedX); break;
   case opSTY_abs: StoreRegister(regY, addrAbsolute); break;

      // transfer opcodes
   case opTAX: TransferRegister(regA, regX); break;
   case opTXA: TransferRegister(regX, regA); break;
   case opTAY: TransferRegister(regA, regY); break;
   case opTYA: TransferRegister(regY, regA); break;
   case opTSX: TransferRegister(regSP, regX); break;
   case opTXS: TransferRegister(regX, regSP); break;

      // inc/dec opcodes
   case opDEC_zp:  IncDecRegister(regA, addrZeropage, false); break;
   case opDEC_zpx: IncDecRegister(regA, addrZeropageIndexedX, false); break;
   case opDEC_abs: IncDecRegister(regA, addrAbsolute, false); break;
   case opDEC_abx: IncDecRegister(regA, addrAbsoluteIndexedX, false); break;
   case opDEX:     IncDecRegister(regX, addrImplicit, false); break;
   case opDEY:     IncDecRegister(regY, addrImplicit, false); break;

   case opINC_zp:  IncDecRegister(regA, addrZeropage, true); break;
   case opINC_zpx: IncDecRegister(regA, addrZeropageIndexedX, true); break;
   case opINC_abs: IncDecRegister(regA, addrAbsolute, true); break;
   case opINC_abx: IncDecRegister(regA, addrAbsoluteIndexedX, true); break;
   case opINX:     IncDecRegister(regX, addrImplicit, true); break;
   case opINY:     IncDecRegister(regY, addrImplicit, true); break;

      // compare case opcodes
   case opCMP_imm: CompareRegister(regA, addrImmediate); break;
   case opCMP_zp:  CompareRegister(regA, addrZeropage); break;
   case opCMP_zpx: CompareRegister(regA, addrZeropageIndexedX); break;
   case opCMP_izx: CompareRegister(regA, addrIndirectZeropageIndexedX); break;
   case opCMP_izy: CompareRegister(regA, addrIndirectZeropageIndexedY); break;
   case opCMP_abs: CompareRegister(regA, addrAbsolute); break;
   case opCMP_abx: CompareRegister(regA, addrAbsoluteIndexedX); break;
   case opCMP_aby: CompareRegister(regA, addrAbsoluteIndexedY); break;

   case opCPX_imm: CompareRegister(regX, addrImmediate); break;
   case opCPX_zp:  CompareRegister(regX, addrZeropage); break;
   case opCPX_abs: CompareRegister(regX, addrAbsolute); break;

   case opCPY_imm: CompareRegister(regY, addrImmediate); break;
   case opCPY_zp:  CompareRegister(regY, addrZeropage); break;
   case opCPY_abs: CompareRegister(regY, addrAbsolute); break;

      // branch opcodes
   case opBPL:
   case opBMI:
   case opBVC:
   case opBVS:
   case opBCC:
   case opBCS:
   case opBNE:
   case opBEQ:
      Branch(bOpcode);
      break;

      // arithmetic opcodes
   case opORA_imm: LogicalOperation(logicalOperationOra, addrImmediate); break;
   case opORA_zp:  LogicalOperation(logicalOperationOra, addrZeropage); break;
   case opORA_zpx: LogicalOperation(logicalOperationOra, addrZeropageIndexedX); break;
   case opORA_izx: LogicalOperation(logicalOperationOra, addrIndirectZeropageIndexedX); break;
   case opORA_izy: LogicalOperation(logicalOperationOra, addrIndirectZeropageIndexedY); break;
   case opORA_abs: LogicalOperation(logicalOperationOra, addrAbsolute); break;
   case opORA_abx: LogicalOperation(logicalOperationOra, addrAbsoluteIndexedX); break;
   case opORA_aby: LogicalOperation(logicalOperationOra, addrAbsoluteIndexedY); break;

   case opAND_imm: LogicalOperation(logicalOperationAnd, addrImmediate); break;
   case opAND_zp : LogicalOperation(logicalOperationAnd, addrZeropage); break;
   case opAND_zpx: LogicalOperation(logicalOperationAnd, addrZeropageIndexedX); break;
   case opAND_izx: LogicalOperation(logicalOperationAnd, addrIndirectZeropageIndexedX); break;
   case opAND_izy: LogicalOperation(logicalOperationAnd, addrIndirectZeropageIndexedY); break;
   case opAND_abs: LogicalOperation(logicalOperationAnd, addrAbsolute); break;
   case opAND_abx: LogicalOperation(logicalOperationAnd, addrAbsoluteIndexedX); break;
   case opAND_aby: LogicalOperation(logicalOperationAnd, addrAbsoluteIndexedY); break;

   case opEOR_imm: LogicalOperation(logicalOperationEor, addrImmediate); break;
   case opEOR_zp : LogicalOperation(logicalOperationEor, addrZeropage); break;
   case opEOR_zpx: LogicalOperation(logicalOperationEor, addrZeropageIndexedX); break;
   case opEOR_izx: LogicalOperation(logicalOperationEor, addrIndirectZeropageIndexedX); break;
   case opEOR_izy: LogicalOperation(logicalOperationEor, addrIndirectZeropageIndexedY); break;
   case opEOR_abs: LogicalOperation(logicalOperationEor, addrAbsolute); break;
   case opEOR_abx: LogicalOperation(logicalOperationEor, addrAbsoluteIndexedX); break;
   case opEOR_aby: LogicalOperation(logicalOperationEor, addrAbsoluteIndexedY); break;

   case opADC_imm: ArithmeticOperation(arithmeticOperationAdc, addrImmediate); break;
   case opADC_zp : ArithmeticOperation(arithmeticOperationAdc, addrZeropage); break;
   case opADC_zpx: ArithmeticOperation(arithmeticOperationAdc, addrZeropageIndexedX); break;
   case opADC_izx: ArithmeticOperation(arithmeticOperationAdc, addrIndirectZeropageIndexedX); break;
   case opADC_izy: ArithmeticOperation(arithmeticOperationAdc, addrIndirectZeropageIndexedY); break;
   case opADC_abs: ArithmeticOperation(arithmeticOperationAdc, addrAbsolute); break;
   case opADC_abx: ArithmeticOperation(arithmeticOperationAdc, addrAbsoluteIndexedX); break;
   case opADC_aby: ArithmeticOperation(arithmeticOperationAdc, addrAbsoluteIndexedY); break;

   case opSBC_imm: ArithmeticOperation(arithmeticOperationSbc, addrImmediate); break;
   case opSBC_zp : ArithmeticOperation(arithmeticOperationSbc, addrZeropage); break;
   case opSBC_zpx: ArithmeticOperation(arithmeticOperationSbc, addrZeropageIndexedX); break;
   case opSBC_izx: ArithmeticOperation(arithmeticOperationSbc, addrIndirectZeropageIndexedX); break;
   case opSBC_izy: ArithmeticOperation(arithmeticOperationSbc, addrIndirectZeropageIndexedY); break;
   case opSBC_abs: ArithmeticOperation(arithmeticOperationSbc, addrAbsolute); break;
   case opSBC_abx: ArithmeticOperation(arithmeticOperationSbc, addrAbsoluteIndexedX); break;
   case opSBC_aby: ArithmeticOperation(arithmeticOperationSbc, addrAbsoluteIndexedY); break;

      // shift / roll opcodes
   case opASL_imp: ShiftOperation(shiftOperationAsl, addrImplicit); break;
   case opASL_zp : ShiftOperation(shiftOperationAsl, addrZeropage); break;
   case opASL_zpx: ShiftOperation(shiftOperationAsl, addrZeropageIndexedX); break;
   case opASL_abs: ShiftOperation(shiftOperationAsl, addrAbsolute); break;
   case opASL_abx: ShiftOperation(shiftOperationAsl, addrAbsoluteIndexedX); break;

   case opROL_imp: ShiftOperation(shiftOperationRol, addrImplicit); break;
   case opROL_zp : ShiftOperation(shiftOperationRol, addrZeropage); break;
   case opROL_zpx: ShiftOperation(shiftOperationRol, addrZeropageIndexedX); break;
   case opROL_abs: ShiftOperation(shiftOperationRol, addrAbsolute); break;
   case opROL_abx: ShiftOperation(shiftOperationRol, addrAbsoluteIndexedX); break;

   case opLSR_imp: ShiftOperation(shiftOperationLsr, addrImplicit); break;
   case opLSR_zp : ShiftOperation(shiftOperationLsr, addrZeropage); break;
   case opLSR_zpx: ShiftOperation(shiftOperationLsr, addrZeropageIndexedX); break;
   case opLSR_abs: ShiftOperation(shiftOperationLsr, addrAbsolute); break;
   case opLSR_abx: ShiftOperation(shiftOperationLsr, addrAbsoluteIndexedX); break;

   case opROR_imp: ShiftOperation(shiftOperationRor, addrImplicit); break;
   case opROR_zp : ShiftOperation(shiftOperationRor, addrZeropage); break;
   case opROR_zpx: ShiftOperation(shiftOperationRor, addrZeropageIndexedX); break;
   case opROR_abs: ShiftOperation(shiftOperationRor, addrAbsolute); break;
   case opROR_abx: ShiftOperation(shiftOperationRor, addrAbsoluteIndexedX); break;

      // push/pop
   case opPHP:
      if (m_debugOutput)
         m_opcodeText.AppendFormat(_T("PHP"));
      Push(GetRegister(regSR) | c_bStatusMaskAlwaysSet);
      break;

   case opPLP:
      if (m_debugOutput)
         m_opcodeText.AppendFormat(_T("PLP"));
      SetRegister(regSR, Pop() | c_bStatusMaskAlwaysSet);
      break;

   case opPHA:
      if (m_debugOutput)
         m_opcodeText.AppendFormat(_T("PHA"));
      Push(GetRegister(regA));
      break;

   case opPLA:
   {
      if (m_debugOutput)
         m_opcodeText.AppendFormat(_T("PLA"));
      BYTE bValue = Pop();
      SetRegister(regA, bValue);

      // set flags
      SetFlag(flagZero, bValue == 0);
      SetFlag(flagNegative, (bValue & 0x80) != 0);
   }
   break;

   // flags
   case opCLC: SetFlagOperation(flagCarry, false); break;
   case opSEC: SetFlagOperation(flagCarry, true); break;
   case opCLD: SetFlagOperation(flagDecimal, false); break;
   case opSED: SetFlagOperation(flagDecimal, true); break;
   case opCLI: SetFlagOperation(flagInterrupt, false); break;
   case opSEI: SetFlagOperation(flagInterrupt, true); break;
   case opCLV: SetFlagOperation(flagOverflow, false); break;

      // control flow opcodes
   case opJMP_abs:
      if (m_debugOutput)
         m_opcodeText.AppendFormat(_T("JMP "));
      m_wProgramCounter = FetchAddress(addrAbsolute);
      bProgramCounterChanged = true;
      break;

   case opJMP_ind:
      if (m_debugOutput)
         m_opcodeText.AppendFormat(_T("JMP "));
      m_wProgramCounter = FetchAddress(addrIndirect);
      if (m_debugOutput)
         m_opcodeText.AppendFormat(_T("(to $%04x)"), m_wProgramCounter);
      bProgramCounterChanged = true;
      break;

   case opJSR:
   {
      WORD wAddr = Load16(m_wProgramCounter);

      m_wProgramCounter++;
      Push(static_cast<BYTE>(m_wProgramCounter >> 8));
      Push(static_cast<BYTE>(m_wProgramCounter & 0xff));
      m_wProgramCounter++;

      m_wProgramCounter = wAddr;
      if (m_debugOutput)
         m_opcodeText.AppendFormat(_T("JSR $%04x"), m_wProgramCounter);
      bProgramCounterChanged = true;
   }
   break;

   case opRTS:
      if (m_debugOutput)
         m_opcodeText.AppendFormat(_T("RTS"));
      Return();
      bProgramCounterChanged = true;
      break;

   case opBRK:
      if (m_debugOutput)
         m_opcodeText.AppendFormat(_T("BRK"));
      m_wProgramCounter++;
      Push(static_cast<BYTE>(m_wProgramCounter >> 8));
      Push(static_cast<BYTE>(m_wProgramCounter & 0xff));

      // note: bit 4 (break) set by this instruction explicitly; it's clear on non-BRK interrupts
      Push(GetRegister(regSR) | c_bStatusMaskAlwaysSet);

      // set interrupt bit, so that no further interrupts may occur
      SetFlag(C64::flagInterrupt, true);
      m_wProgramCounter = Load16(0xfffe);
      bProgramCounterChanged = true;
      break;

   case opRTI:
      if (m_debugOutput)
         m_opcodeText.AppendFormat(_T("RTI"));
      SetRegister(regSR, Pop() | c_bStatusMaskAlwaysSet);
      Return();
      m_wProgramCounter--; // undo the increment in Return()
      bProgramCounterChanged = true;
      break;

   case opBIT_zp:
      BitOperation(addrZeropage);
      break;

   case opBIT_abs:
      BitOperation(addrAbsolute);
      break;

   case 0x1a:
   case 0x3a:
   case 0x5a:
   case 0x7a:
   case 0xda:
   case 0xfa:
   case opNOP: // do nothing
      if (m_debugOutput)
         m_opcodeText.AppendFormat(_T("NOP"));
      break;

   default:
      ATLTRACE(_T("unknown opcode: %02x"), bOpcode);
      break;
   }

   //m_opcodeText.AppendFormat(_T("   \tA=%02x X=%02x Y=%02x P=%02x S=%02x"),
   //   GetRegister(regA),
   //   GetRegister(regX),
   //   GetRegister(regY),
   //   GetRegister(regSR),
   //   GetRegister(regSP));

   if (m_debugOutput)
      m_opcodeText.AppendFormat(_T("\n"));

   if (m_debugOutput)
      ATLTRACE(m_opcodeText);

   if (bProgramCounterChanged)
   {
      std::set<IProcessorCallback*>::const_iterator iter, stop;
      iter = m_setCallbacks.begin(); stop = m_setCallbacks.end();
      for(;iter != stop; iter++)
         (*iter)->OnProgramCounterChange();
   }
}

void Processor6510::Return()
{
   BYTE bRetLow = Pop();
   BYTE bRetHigh = Pop();
   m_wProgramCounter = LE2WORD(bRetLow, bRetHigh) + 1;
}

void Processor6510::Interrupt(C64::InterruptType enInterruptType)
{
   ATLASSERT(enInterruptType == interruptTypeIRQ_VIC); // only VIC interrupt is supported
   enInterruptType;

   // push addr of next opcode
   WORD wAddr = m_wProgramCounter;
   Push(static_cast<BYTE>(wAddr >> 8));
   Push(static_cast<BYTE>(wAddr & 0xff));

   // note: bit 4 (break) is unset
   Push(GetRegister(regSR) | c_bStatusMaskAlwaysSet & (~C64::flagBreak));

   // set interrupt bit, so that no further interrupts may occur
   SetFlag(C64::flagInterrupt, true);
   m_wProgramCounter = m_memoryManager.Peek16(0xfffe);
}

BYTE Processor6510::Load(WORD wAddr)
{
   return m_memoryManager.Peek(wAddr);
}

void Processor6510::Store(WORD wAddr, BYTE bValue)
{
   m_memoryManager.Poke(wAddr, bValue);
}

WORD Processor6510::FetchAddress(AddressingMode enAddressingMode)
{
   WORD wAddr = m_wProgramCounter++;
   BYTE bArg1 = Load(wAddr);

   switch (enAddressingMode)
   {
   case addrImmediate:
      //wAddr = wAddr;
      if (m_debugOutput)
         m_opcodeText.AppendFormat(_T("#$%02x"), bArg1);
      break;

   case addrZeropage:
      wAddr = static_cast<WORD>(bArg1);
      if (m_debugOutput)
         m_opcodeText.AppendFormat(_T("$%02x"), bArg1);
      break;

   case addrAbsolute:
      wAddr = LE2WORD(bArg1, Load(m_wProgramCounter++));
      if (m_debugOutput)
         m_opcodeText.AppendFormat(_T("$%04x"), wAddr);
      break;

   case addrZeropageIndexedX: // zpx = $00,X
      // note: adding x wraps around on zeropage
      wAddr = (static_cast<WORD>(bArg1) + GetRegister(regX)) & 0xFF;
      if (m_debugOutput)
         m_opcodeText.AppendFormat(_T("$%02x,x"), bArg1);
      break;

   case addrZeropageIndexedY: // zpy = $00,Y
      // note: adding y wraps around on zeropage
      wAddr = (static_cast<WORD>(bArg1) + GetRegister(regY)) & 0xFF;
      if (m_debugOutput)
         m_opcodeText.AppendFormat(_T("$%02x,y"), bArg1);
      break;

   case addrIndirectZeropageIndexedX: // izx = ($00,X)
      wAddr = Load16((bArg1 + GetRegister(regX)) & 0xff);
      if (m_debugOutput)
         m_opcodeText.AppendFormat(_T("($%02x,x)"), bArg1);
      break;

   case addrIndirectZeropageIndexedY: // izy = ($00),Y
      wAddr = Load16(static_cast<WORD>(bArg1)) + GetRegister(regY);
      if (m_debugOutput)
         m_opcodeText.AppendFormat(_T("($%02x),y"), bArg1);
      break;

   case addrAbsoluteIndexedX:
      wAddr = LE2WORD(bArg1, Load(m_wProgramCounter++));
      if (m_debugOutput)
         m_opcodeText.AppendFormat(_T("$%04x,x"), wAddr);
      wAddr = wAddr + GetRegister(regX);
      break;

   case addrAbsoluteIndexedY:
      wAddr = LE2WORD(bArg1, Load(m_wProgramCounter++));
      if (m_debugOutput)
         m_opcodeText.AppendFormat(_T("$%04x,y"), wAddr);
      wAddr = wAddr + GetRegister(regY);
      break;

   case addrIndirect:
   {
      BYTE bArg2 = Load(m_wProgramCounter++);
      wAddr = LE2WORD(bArg1, bArg2);

      if (m_debugOutput)
         m_opcodeText.AppendFormat(_T("($%04x)"), wAddr);

      BYTE bArg3 = Load(wAddr++);

      if ((wAddr & 0xff) == 0) // on page boundary, fetch byte from previous page
         wAddr -= 0x0100;

      bArg2 = Load(wAddr);
      wAddr = LE2WORD(bArg3, bArg2);
   }
   break;

   case addrRelative:
      if (bArg1 < 0x80)
         wAddr = m_wProgramCounter + bArg1;
      else
         wAddr = m_wProgramCounter - (bArg1 ^ 0xff) - 1;
      break;

   default:
      ATLASSERT(false);
      break;
   }

   return wAddr;
}

void Processor6510::Push(BYTE bData)
{
   BYTE bSP = GetRegister(regSP);
   Store(0x0100 + bSP, bData);
   SetRegister(regSP, (bSP-1) & 0xff);
}

BYTE Processor6510::Pop()
{
   BYTE bSP = GetRegister(regSP);
   bSP = (bSP + 1) & 0xff;
   SetRegister(regSP, bSP);

   BYTE bValue = Load(0x0100 + bSP);
   return bValue;
}

void Processor6510::LoadRegister(RegisterID enRegisterID, AddressingMode enAddressingMode)
{
   if (m_debugOutput)
   {
      switch (enRegisterID)
      {
      case regA: m_opcodeText.AppendFormat(_T("LDA ")); break;
      case regX: m_opcodeText.AppendFormat(_T("LDX ")); break;
      case regY: m_opcodeText.AppendFormat(_T("LDY ")); break;
      default:
         ATLASSERT(false);
         break;
      };
   }

   WORD wAddr = FetchAddress(enAddressingMode);
   BYTE bValue = Load(wAddr);

   // hide "from" addr in some addressing modes
   if (m_debugOutput)
   {
      if (enAddressingMode != addrImmediate &&
         enAddressingMode != addrZeropage &&
         enAddressingMode != addrAbsolute)
         m_opcodeText.AppendFormat(_T(" (from $%04x)"), wAddr);
   }

   SetRegister(enRegisterID, bValue);

   // set status flags
   SetFlag(flagZero, bValue == 0);
   SetFlag(flagNegative, (bValue & 0x80) != 0);

   // count cycles
   if (enRegisterID == regA)
   {
      // add extra cycle when page boundary is crossed
      if ((enAddressingMode == addrIndirectZeropageIndexedY && enRegisterID == regA) ||
          (enAddressingMode == addrAbsoluteIndexedX && (enRegisterID == regA || enRegisterID == regY)) ||
          (enAddressingMode == addrAbsoluteIndexedY && (enRegisterID == regA || enRegisterID == regX)))
      {
         BYTE bReg = GetRegister(enAddressingMode == addrAbsoluteIndexedX ? regX : regY);

         // page boundary was crossed while adding register?
         if ((wAddr & 0xff00) != ((wAddr - bReg) & 0xff00))
            m_uiElapsedCycles++;
      }
   }
}

void Processor6510::StoreRegister(RegisterID enRegisterID, AddressingMode enAddressingMode)
{
   ATLASSERT(enAddressingMode != addrImmediate);

   if (m_debugOutput)
   {
      switch (enRegisterID)
      {
      case regA: m_opcodeText.AppendFormat(_T("STA ")); break;
      case regX: m_opcodeText.AppendFormat(_T("STX ")); break;
      case regY: m_opcodeText.AppendFormat(_T("STY ")); break;
      default:
         ATLASSERT(false);
         break;
      };
   }

   BYTE bValue = GetRegister(enRegisterID);
   WORD wAddr = FetchAddress(enAddressingMode);

   if (m_debugOutput)
   {
      // hide "to" addr in some addressing modes
      if (enAddressingMode != addrZeropage &&
         enAddressingMode != addrAbsolute)
         m_opcodeText.AppendFormat(_T(" (to $%04x)"), wAddr);
   }

   Store(wAddr, bValue);
}

void Processor6510::TransferRegister(RegisterID enRegSource, RegisterID enRegTarget)
{
   if (m_debugOutput)
   {
      m_opcodeText.AppendFormat(
         _T("T%c%c"),
         enRegSource == regA ? _T('A') : enRegSource == regX ? _T('X') : enRegSource == regY ? _T('X') : _T('S'),
         enRegTarget == regA ? _T('A') : enRegSource == regX ? _T('X') : enRegSource == regY ? _T('X') : _T('S'));
   }

   BYTE bValue = GetRegister(enRegSource);
   SetRegister(enRegTarget, bValue);

   // set status flags; not when stack pointer is target
   if (enRegTarget != regSP)
   {
      SetFlag(flagZero, bValue == 0);
      SetFlag(flagNegative, (bValue & 0x80) != 0);
   }
}

void Processor6510::IncDecRegister(RegisterID enRegister, AddressingMode enAddressingMode, bool bIncrease)
{
   BYTE bValue;
   if (enRegister == regA) // accumulator really means "modify memory"
   {
      if (m_debugOutput)
         m_opcodeText.AppendFormat(bIncrease ? _T("INC ") : _T("DEC "));

      // only some addressing modes are allowed
      ATLASSERT(enAddressingMode == addrZeropage ||
         enAddressingMode == addrZeropageIndexedX ||
         enAddressingMode == addrAbsolute ||
         enAddressingMode == addrAbsoluteIndexedX);

      WORD wAddr = FetchAddress(enAddressingMode);
      bValue = Load(wAddr);
      bValue += bIncrease ? 1 : 0xFF;
      Store(wAddr, bValue);
   }
   else
      if (enRegister == regX || enRegister == regY)
      {
         ATLASSERT(enAddressingMode == addrImplicit);
         bValue = GetRegister(enRegister);
         bValue += bIncrease ? 1 : 0xFF;
         SetRegister(enRegister, bValue);

         if (m_debugOutput)
         {
            m_opcodeText.AppendFormat(_T("%s%c         (new value #$%02x)"),
               bIncrease ? _T("IN") : _T("DE"),
               enRegister == regX ? _T('X') : _T('Y'),
               bValue);
         }
      }
      else
      {
         ATLASSERT(false);
         return;
      }

   // set status register
   SetFlag(flagZero, bValue == 0);
   SetFlag(flagNegative, (bValue & 0x80) != 0);
}

void Processor6510::CompareRegister(RegisterID enRegisterID, AddressingMode enAddressingMode)
{
   BYTE bValueReg = GetRegister(enRegisterID);

   if (m_debugOutput)
   {
      switch (enRegisterID)
      {
      case regA: m_opcodeText.AppendFormat(_T("CMP ")); break;
      case regX: m_opcodeText.AppendFormat(_T("CPX ")); break;
      case regY: m_opcodeText.AppendFormat(_T("CPY ")); break;
      default:
         ATLASSERT(false);
         break;
      };
   }

   // some addressing modes are not allowed for CPX and CPY
   if (enAddressingMode == addrZeropageIndexedX ||
       enAddressingMode == addrIndirectZeropageIndexedX ||
       enAddressingMode == addrIndirectZeropageIndexedY ||
       enAddressingMode == addrAbsoluteIndexedX ||
       enAddressingMode == addrAbsoluteIndexedY)
      ATLASSERT(enRegisterID == regA); // addressing mode not allowed with x or y register

   WORD wAddr = FetchAddress(enAddressingMode);
   BYTE bValue2 = Load(wAddr);

   // set flags
   BYTE bResult = bValueReg - bValue2;
   SetFlag(flagZero, bResult == 0);
   SetFlag(flagNegative, bResult >= 0x80);
   SetFlag(flagCarry, bValueReg >= bValue2);

   if (enRegisterID == regA)
   {
      // count cycles; add extra cycle when page boundary is crossed
      if (enAddressingMode == addrIndirectZeropageIndexedY ||
          enAddressingMode == addrAbsoluteIndexedX ||
          enAddressingMode == addrAbsoluteIndexedY)
      {
         BYTE bReg = GetRegister(enAddressingMode == addrAbsoluteIndexedX ? regX : regY);

         // page boundary was crossed while adding register?
         if ((wAddr & 0xff00) != ((wAddr - bReg) & 0xff00))
            m_uiElapsedCycles++;
      }
   }
}

void Processor6510::Branch(BYTE bOpcode)
{
   WORD wLastPC = m_wProgramCounter;

   bool bNegate = false;
   C64::ProcessorFlag flag;

   if (m_debugOutput)
   {
      switch (bOpcode)
      {
         // branch opcodes
      case opBPL: m_opcodeText.AppendFormat(_T("BPL ")); break;
      case opBMI: m_opcodeText.AppendFormat(_T("BMI ")); break;
      case opBVC: m_opcodeText.AppendFormat(_T("BVC ")); break;
      case opBVS: m_opcodeText.AppendFormat(_T("BVS ")); break;
      case opBCC: m_opcodeText.AppendFormat(_T("BCC ")); break;
      case opBCS: m_opcodeText.AppendFormat(_T("BCS ")); break;
      case opBNE: m_opcodeText.AppendFormat(_T("BNE ")); break;
      case opBEQ: m_opcodeText.AppendFormat(_T("BEQ ")); break;
      default:
         ATLASSERT(false);
         return;
      }
   }

   switch (bOpcode)
   {
      // branch opcodes
   case opBPL: flag = flagNegative; bNegate = true; break; // branch when negative clear
   case opBMI: flag = flagNegative;                 break; // branch when negative set
   case opBVC: flag = flagOverflow; bNegate = true; break;
   case opBVS: flag = flagOverflow;                 break;
   case opBCC: flag = flagCarry;    bNegate = true; break;
   case opBCS: flag = flagCarry;                    break;
   case opBNE: flag = flagZero;     bNegate = true; break; // branch when zero clear
   case opBEQ: flag = flagZero;                     break; // branch when zero set
   default:
      ATLASSERT(false);
      return;
   }

   bool bBranch = GetFlag(flag);
   if (bNegate)
      bBranch = !bBranch;

   BYTE bDistance = Load(m_wProgramCounter);
   WORD wAddr = FetchAddress(addrRelative);

   if (m_debugOutput)
      m_opcodeText.AppendFormat(_T("$%04x   ($%02x)"), wAddr, bDistance);

   if (bBranch)
   {
      m_wProgramCounter = wAddr;

      // branch taken: add one cycle
      m_uiElapsedCycles++;

      // page crossed? then add another one
      if ((wLastPC && 0xff00) != (m_wProgramCounter & 0xff00))
         m_uiElapsedCycles++;
   }
   else
   {
      if (m_debugOutput)
         m_opcodeText.AppendFormat(_T(" (not taken)"));
   }
}

void Processor6510::LogicalOperation(C64::LogicalOperation enLogicalOperation, AddressingMode enAddressingMode)
{
   if (m_debugOutput)
   {
      switch (enLogicalOperation)
      {
      case logicalOperationOra: m_opcodeText.AppendFormat(_T("ORA ")); break;
      case logicalOperationAnd: m_opcodeText.AppendFormat(_T("AND ")); break;
      case logicalOperationEor: m_opcodeText.AppendFormat(_T("EOR ")); break;
      default:
         ATLASSERT(false);
      }
   }

   BYTE bOperand1 = GetRegister(regA);

   WORD wAddr = FetchAddress(enAddressingMode);
   BYTE bOperand2 = Load(wAddr);

   BYTE bResult;
   switch (enLogicalOperation)
   {
   case logicalOperationOra:
      bResult = bOperand1 | bOperand2;
      break;

   case logicalOperationAnd:
      bResult = bOperand1 & bOperand2;
      break;

   case logicalOperationEor:
      bResult = bOperand1 ^ bOperand2;
      break;

   default:
      ATLASSERT(false);
      return;
   }

   SetRegister(regA, bResult);

   // set flags
   SetFlag(flagZero, bResult == 0);
   SetFlag(flagNegative, (bResult & 0x80) != 0);

   // count cycles; add extra cycle when page boundary is crossed
   if (enAddressingMode == addrIndirectZeropageIndexedY ||
       enAddressingMode == addrAbsoluteIndexedX ||
       enAddressingMode == addrAbsoluteIndexedY)
   {
      BYTE bReg = GetRegister(enAddressingMode == addrAbsoluteIndexedX ? regX : regY);

      // page boundary was crossed while adding register?
      if ((wAddr & 0xff00) != ((wAddr - bReg) & 0xff00))
         m_uiElapsedCycles++;
   }
}

void Processor6510::ArithmeticOperation(C64::ArithmeticOperation enArithmeticOperation, AddressingMode enAddressingMode)
{
   if (m_debugOutput)
   {
      switch (enArithmeticOperation)
      {
      case arithmeticOperationAdc: m_opcodeText.AppendFormat(_T("ADC ")); break;
      case arithmeticOperationSbc: m_opcodeText.AppendFormat(_T("SBC ")); break;
      default:
         ATLASSERT(false);
      }
   }

   BYTE bOperand1 = GetRegister(regA);

   WORD wAddr = FetchAddress(enAddressingMode);
   BYTE bOperand2 = Load(wAddr);

   BYTE bResult;
   switch (enArithmeticOperation)
   {
   case arithmeticOperationAdc:
      {
         WORD wResult;
         if (!GetFlag(flagDecimal))
         {
            wResult = bOperand1 + bOperand2 + (GetFlag(flagCarry) ? 1 : 0);
            bResult = static_cast<BYTE>(wResult & 0xff);

            // set flags
            SetFlag(flagCarry, wResult > 0xff);

            // change of sign?
            bool bOverflow = !((bOperand1 ^ (bOperand2)) & 0x80) && ((bOperand1 ^ bResult) & 0x80);
            SetFlag(flagOverflow, bOverflow);

            SetFlag(flagZero, bResult == 0);
            SetFlag(flagNegative, (bResult & 0x80) != 0);
         }
         else
         {
            // decimal mode

            // calculate the lower and upper nibbles separately
            BYTE bLow = (bOperand1 & 0x0f) + (bOperand2 & 0x0f) + (GetFlag(flagCarry) ? 1 : 0);

            BYTE bHigh = (bOperand1 >> 4) + (bOperand2 >> 4) + (bLow > 9 ? 1 : 0);

            // BCD fixup for lower nibble
            if (bLow > 9)
               bLow += 6;

            // set flags

            // zero flag is set just like in binary mode
            WORD wResultBin = bOperand1 + bOperand2 + (GetFlag(flagCarry) ? 1 : 0);
            SetFlag(flagZero, (wResultBin & 0xff) == 0); 

            // negative and overflow flags are set with the same logic as in
            // binary mode, but after fixing the lower nibble
            SetFlag(flagNegative, (bHigh & 8) != 0);

            bool bOverflow = (((bHigh << 4) ^ bOperand1) & 0x80) && !((bOperand1 ^ bOperand2) & 0x80);
            SetFlag(flagOverflow, bOverflow);

            // BCD fixup for upper nibble
            if (bHigh > 9)
               bHigh += 6;

            // carry is the only flag set after fixing the result
            SetFlag(flagCarry, bHigh > 15);

            wResult = (bLow & 0x0f) | (static_cast<WORD>(bHigh) << 4);
            bResult = static_cast<BYTE>(wResult & 0xff);
         }
      }
      break;

   case arithmeticOperationSbc:
      {
         if (!GetFlag(flagDecimal))
         {
            WORD wResult = bOperand1 - bOperand2 - (GetFlag(flagCarry) ? 0 : 1);
            bResult = static_cast<BYTE>(wResult & 0xff);

            // set flags
            SetFlag(flagZero, bResult == 0);
            SetFlag(flagNegative, (bResult & 0x80) != 0);
            SetFlag(flagCarry, wResult < 0x100); // underflow when result is 0xff??

            bool bOverflow = ((bOperand1 ^ wResult) & 0x80) && ((bOperand1 ^ bOperand2) & 0x80);
            SetFlag(flagOverflow, bOverflow);
         }
         else
         {
            // decimal mode

            // calculate lower nibble
            BYTE bLow = (bOperand1 & 0xf) - (bOperand2 & 0xf) - (GetFlag(flagCarry) ? 0 : 1);

            // calculate upper nibble
            //BYTE bHigh = (bOperand1 >> 4) - (bOperand2 >> 4) - ((bLow & 16) != 0 ? 1 : 0);
            BYTE bHigh = (bOperand1 >> 4) - (bOperand2 >> 4) - ((bLow & 16) != 0 ? 1 : 0);

            // BCD fixup for lower nibble
            if (bLow & 0x10)
               bLow -= 6;

            // BCD fixup for upper nibble
            if (bHigh & 0x10)
               bHigh -= 6;

            // set flags
            WORD wResultBin = (bOperand1 - bOperand2 - (GetFlag(flagCarry) ? 0 : 1));

            SetFlag(flagZero, (wResultBin & 0xff) == 0);
            SetFlag(flagNegative, (wResultBin & 0x80) != 0);
            SetFlag(flagCarry, wResultBin < 0x100);

            bool bOverflow = ((bOperand1 ^ wResultBin) & 0x80) && ((bOperand1 ^ bOperand2) & 0x80);
            SetFlag(flagOverflow, bOverflow);

            WORD wResult = (bLow & 0x0f) | (static_cast<WORD>(bHigh) << 4);
            bResult = static_cast<BYTE>(wResult & 0xff);
         }
      }
      break;

   default:
      ATLASSERT(false);
      return;
   }

   SetRegister(regA, bResult);

   // count cycles; add extra cycle when page boundary is crossed
   if (enAddressingMode == addrIndirectZeropageIndexedY ||
       enAddressingMode == addrAbsoluteIndexedX ||
       enAddressingMode == addrAbsoluteIndexedY)
   {
      BYTE bReg = GetRegister(enAddressingMode == addrAbsoluteIndexedX ? regX : regY);

      // page boundary was crossed while adding register?
      if ((wAddr & 0xff00) != ((wAddr - bReg) & 0xff00))
         m_uiElapsedCycles++;
   }
}

void Processor6510::ShiftOperation(C64::ShiftOperation enShiftOperation, AddressingMode enAddressingMode)
{
   if (m_debugOutput)
   {
      switch (enShiftOperation)
      {
      case shiftOperationAsl: m_opcodeText.AppendFormat(_T("ASL ")); break;
      case shiftOperationRol: m_opcodeText.AppendFormat(_T("ROL ")); break;
      case shiftOperationLsr: m_opcodeText.AppendFormat(_T("LSR ")); break;
      case shiftOperationRor: m_opcodeText.AppendFormat(_T("ROR ")); break;
      default:
         ATLASSERT(false);
         return;
      }
   }

   WORD wAddr = enAddressingMode == addrImplicit ? 0 : FetchAddress(enAddressingMode);
   BYTE bOperand = enAddressingMode == addrImplicit ? GetRegister(regA) : Load(wAddr);

   BYTE bResult;
   bool bCarry = GetFlag(flagCarry);
   switch (enShiftOperation)
   {
   case shiftOperationAsl: // shift left; c = op.bit7, a = op * 2;
      bCarry = (bOperand & 0x80) != 0; // bit 7 -> carry
      bResult = bOperand << 1;
      break;

   case shiftOperationLsr: // shift right; c = op.bit0, a = op / 2
      bCarry = (bOperand & 1) != 0; // bit 0 -> carry
      bResult = bOperand >> 1;
      break;

   case shiftOperationRol: // roll left; c = op.bit7, a = op * 2 + c
      bResult = (bOperand << 1) | (bCarry ? 1 : 0);
      bCarry = (bOperand & 0x80) != 0; // bit 7 -> carry
      break;

   case shiftOperationRor: // roll right, c = op.bit0, a = op / 2 + 128 * c
      bResult = (bOperand >> 1) | (bCarry ? 0x80 : 0);
      bCarry = (bOperand & 1) != 0; // bit 0 -> carry
      break;

   default:
      ATLASSERT(false);
      return;
   }

   if (enAddressingMode == addrImplicit)
      SetRegister(regA, bResult);
   else
      Store(wAddr, bResult);

   // set flags
   SetFlag(flagZero, bResult == 0);
   SetFlag(flagNegative, (bResult & 0x80) != 0);
   SetFlag(flagCarry, bCarry);
}

void Processor6510::BitOperation(AddressingMode enAddressingMode)
{
   if (m_debugOutput)
      m_opcodeText.AppendFormat(_T("BIT "));

   WORD wAddr = FetchAddress(enAddressingMode);

   BYTE bValue = Load(wAddr);
   BYTE bResult = GetRegister(regA) & bValue;

   // set flags
   SetFlag(flagZero, bResult == 0);
   SetFlag(flagNegative, (bValue & 0x80) != 0);
   SetFlag(flagOverflow, (bValue & 0x40) != 0);
}

void Processor6510::SetFlagOperation(C64::ProcessorFlag enFlag, bool bFlag)
{
   if (m_debugOutput)
   {
      m_opcodeText.AppendFormat(bFlag ? _T("SE") : _T("CL"));

      switch (enFlag)
      {
      case flagCarry: m_opcodeText.AppendFormat(_T("C")); break;
      case flagDecimal: m_opcodeText.AppendFormat(_T("D")); break;
      case flagInterrupt: m_opcodeText.AppendFormat(_T("I")); break;
      case flagOverflow: m_opcodeText.AppendFormat(_T("O")); break;
      default:
         ATLASSERT(false);
         break;
      }
   }

   SetFlag(enFlag, bFlag);
}
