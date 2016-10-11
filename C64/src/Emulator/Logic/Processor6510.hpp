//
// Emulator - Simple C64 emulator
// Copyright (C) 2003-2016 Michael Fink
//
/// \file Processor6510.hpp MOS6510 processor emulation
//
#pragma once

// includes
#include <set>
#include "Common.hpp"

/// c64 emulator namespace
namespace C64
{

class MemoryManager;

/// register ID
enum RegisterID
{
   regA = 0,   ///< accumulator
   regX = 1,   ///< x index register
   regY = 2,   ///< y index register
   regSP = 3,  ///< stack pointer
   regSR = 4,  ///< status register
   regLast = regSR
};

/// processor flag
enum ProcessorFlag
{
   flagCarry = 0,
   flagZero = 1,
   flagInterrupt = 2,
   flagDecimal = 3,
   flagBreak = 4,
   flagUnused = 5,
   flagOverflow = 6,
   flagNegative = 7,
};

/// memory addressing mode
enum AddressingMode
{
   addrImplicit,  ///< imp, opcode has no arguments; addressing is specified in the opcode itself, e.g. INX
   addrImmediate, ///< imm, value is opcode argument; e.g. #$00
   addrZeropage,  ///< zp, value is address into zeropage; e.g. $00
   addrZeropageIndexedX, ///< zpx, value is address into zeropage, offset by X; e.g. $00,X
   addrZeropageIndexedY, ///< zpy, value is address into zeropage, offset by Y; e.g. $00,Y
   addrIndirectZeropageIndexedX, ///< izx; e.g. ($00,X)
   addrIndirectZeropageIndexedY, ///< izy; e.g. ($00),Y
   addrAbsolute,         ///< abs, value is absolute address; e.g. $0000
   addrAbsoluteIndexedX, ///< abx; e.g. $0000,X
   addrAbsoluteIndexedY, ///< aby; e.g. $0000,Y 
   addrIndirect, ///< ind, indirect; e.g. ($0000)
   addrRelative, ///< rel, relative to PC, as signed 8-bit offset; e.g. $0000
};

/// opcodes ordered by command type
enum Opcode
{
   // branch opcodes
   opBPL = 0x10,
   opBMI = 0x30,
   opBVC = 0x50,
   opBVS = 0x70,
   opBCC = 0x90,
   opBCS = 0xb0,
   opBNE = 0xd0,
   opBEQ = 0xf0,

   // processor flag operations
   opCLC = 0x18,
   opSEC = 0x38,
   opCLI = 0x58,
   opSEI = 0x78,
   opCLV = 0xB8,
   opCLD = 0xD8,
   opSED = 0xF8,

   // push/pop operations
   opPHP = 0x08,
   opPLP = 0x28,
   opPHA = 0x48,
   opPLA = 0x68,

   // jump operations
   opJSR = 0x20,
   opJMP_abs = 0x4c,
   opRTS = 0x60,
   opJMP_ind = 0x6c,

   // load operations
   opLDA_imm = 0xa9,
   opLDA_zp  = 0xa5,
   opLDA_zpx = 0xb5,
   opLDA_izx = 0xa1,
   opLDA_izy = 0xb1,
   opLDA_abs = 0xad,
   opLDA_abx = 0xbd,
   opLDA_aby = 0xb9,

   opLDX_imm = 0xa2,
   opLDX_zp  = 0xa6,
   opLDX_zpy = 0xb6,
   opLDX_abs = 0xae,
   opLDX_aby = 0xbe,

   opLDY_imm = 0xa0,
   opLDY_zp  = 0xa4,
   opLDY_zpx = 0xb4,
   opLDY_abs = 0xac,
   opLDY_abx = 0xbc,

   // store operations
   opSTA_zp  = 0x85,
   opSTA_zpx = 0x95,
   opSTA_izx = 0x81,
   opSTA_izy = 0x91,
   opSTA_abs = 0x8d,
   opSTA_abx = 0x9d,
   opSTA_aby = 0x99,

   opSTX_zp  = 0x86,
   opSTX_zpy = 0x96,
   opSTX_abs = 0x8e,

   opSTY_zp  = 0x84,
   opSTY_zpx = 0x94,
   opSTY_abs = 0x8c,

   // transfer register operations
   opTXA = 0x8A,
   opTYA = 0x98,
   opTXS = 0x9a,
   opTAY = 0xA8,
   opTAX = 0xAA,
   opTSX = 0xba,

   // inc/dec operations
   opDEC_zp  = 0xc6,
   opDEC_zpx = 0xd6,
   opDEC_abs = 0xce,
   opDEC_abx = 0xde,
   opDEX = 0xca,
   opDEY = 0x88,

   opINC_zp  = 0xe6,
   opINC_zpx = 0xf6,
   opINC_abs = 0xee,
   opINC_abx = 0xfe,
   opINX = 0xe8,
   opINY = 0xc8,

   // compare operations
   opCMP_imm = 0xc9,
   opCMP_zp = 0xc5,
   opCMP_zpx = 0xd5,
   opCMP_izx = 0xc1,
   opCMP_izy = 0xd1,
   opCMP_abs = 0xcd,
   opCMP_abx = 0xdd,
   opCMP_aby = 0xd9,

   opCPX_imm = 0xe0,
   opCPX_zp  = 0xe4,
   opCPX_abs = 0xec,

   opCPY_imm = 0xc0,
   opCPY_zp  = 0xc4,
   opCPY_abs = 0xcc,

   // logical operations
   opORA_imm = 0x09,
   opORA_zp  = 0x05,
   opORA_zpx = 0x15,
   opORA_izx = 0x01,
   opORA_izy = 0x11,
   opORA_abs = 0x0d,
   opORA_abx = 0x1d,
   opORA_aby = 0x19,

   opAND_imm = 0x29,
   opAND_zp  = 0x25,
   opAND_zpx = 0x35,
   opAND_izx = 0x21,
   opAND_izy = 0x31,
   opAND_abs = 0x2d,
   opAND_abx = 0x3d,
   opAND_aby = 0x39,

   opEOR_imm = 0x49,
   opEOR_zp  = 0x45,
   opEOR_zpx = 0x55,
   opEOR_izx = 0x41,
   opEOR_izy = 0x51,
   opEOR_abs = 0x4d,
   opEOR_abx = 0x5d,
   opEOR_aby = 0x59,

   // arithmetical operations
   opADC_imm = 0x69,
   opADC_zp  = 0x65,
   opADC_zpx = 0x75,
   opADC_izx = 0x61,
   opADC_izy = 0x71,
   opADC_abs = 0x6d,
   opADC_abx = 0x7d,
   opADC_aby = 0x79,

   opSBC_imm = 0xe9,
   opSBC_zp  = 0xe5,
   opSBC_zpx = 0xf5,
   opSBC_izx = 0xe1,
   opSBC_izy = 0xf1,
   opSBC_abs = 0xed,
   opSBC_abx = 0xfd,
   opSBC_aby = 0xf9,

   // shift operations
   opASL_imp = 0x0a,
   opASL_zp  = 0x06,
   opASL_zpx = 0x16,
   opASL_abs = 0x0e,
   opASL_abx = 0x1e,

   opROL_imp = 0x2a,
   opROL_zp  = 0x26,
   opROL_zpx = 0x36,
   opROL_abs = 0x2e,
   opROL_abx = 0x3e,

   opLSR_imp = 0x4a,
   opLSR_zp  = 0x46,
   opLSR_zpx = 0x56,
   opLSR_abs = 0x4e,
   opLSR_abx = 0x5e,

   opROR_imp = 0x6a,
   opROR_zp  = 0x66,
   opROR_zpx = 0x76,
   opROR_abs = 0x6e,
   opROR_abx = 0x7e,

   // bit operations
   opBIT_zp = 0x24,
   opBIT_abs = 0x2c,

   // special opcodes
   opBRK = 0x00,
   opRTI = 0x40,
   opNOP = 0xea,
};

/// logical operation type
enum LogicalOperation
{
   logicalOperationOra, ///< or operation
   logicalOperationAnd, ///< and operation
   logicalOperationEor, ///< xor operation
};

/// arithmetic operation
enum ArithmeticOperation
{
   arithmeticOperationAdc, ///< add with carry operation
   arithmeticOperationSbc, ///< subtract with carry operation
};

/// shift operation
enum ShiftOperation
{
   shiftOperationAsl,   ///< arithmetic shift left operation
   shiftOperationRol,   ///< rotate left operation
   shiftOperationLsr,   ///< logical shift right operation
   shiftOperationRor,   ///< rotate right operation
};

/// interrupt type
enum InterruptType
{
   interruptTypeIRQ_VIC,   ///< video chip interrupt
   interruptTypeIRQ_CIA,   ///< CIA chip interrupt
   interruptTypeNMI,       ///< nonmaskable interrupt
};

/// processor callback interface
class IProcessorCallback
{
public:
   /// dtor
   virtual ~IProcessorCallback() throw() {}

   /// called when PC changes on JMP, JSR, RTS, RTI and interrupts
   virtual void OnProgramCounterChange() {}

   /// called when an opcode step is made
   virtual void OnStep() {}

   /// called when a memory load is made
   virtual void OnLoad(WORD address) { address; }

   /// called when a memory store is made
   virtual void OnStore(WORD address) { address; }

   /// called when an opcode is executed
   virtual void OnExecute(WORD address, unsigned int instructionSize) { address; instructionSize; }
};

/// MOS 6510 processor implementation
/// PAL clock 985.248 kHz
/// \see http://www.cloud9.co.uk/james/BBCMicro/Documentation/64doc.txt
/// \see https://blog.fynydd.com/crash-course-on-emulating-the-mos-6510-cpu/
class Processor6510
{
public:
   /// ctor
   Processor6510(MemoryManager& memoryManager) throw();
   /// dtor
   ~Processor6510() throw() {}

   /// adds processor callback
   void AddProcessorCallback(IProcessorCallback* pCallback);
   /// removes processor callback
   void RemoveProcessorCallback(IProcessorCallback* pCallback);

   /// enables or dislables debug output
   void SetDebugOutput(bool enable) { m_debugOutput = enable; }

   /// returns current program counter
   WORD GetProgramCounter() const throw() { return m_wProgramCounter; }

   /// sets new program counter
   void SetProgramCounter(WORD wProgramCounter)
   {
      m_wProgramCounter = wProgramCounter;
   }

   /// returns processor flag
   bool GetFlag(ProcessorFlag enFlag);

   /// sets processor flag
   void SetFlag(ProcessorFlag enFlag, bool bFlag);

   /// returns processor register
   BYTE GetRegister(RegisterID enRegisterID)
   {
      ATLASSERT(enRegisterID <= regLast);
      return m_abRegister[enRegisterID];
   }

   /// sets processor register
   void SetRegister(RegisterID enRegisterID, BYTE bValue)
   {
      ATLASSERT(enRegisterID <= regLast);
      m_abRegister[enRegisterID] = bValue;
   }

   /// executes an opcode
   void Step();

   /// pushes a byte to the stack
   void Push(BYTE bData);
   /// pops a byte from the stack
   BYTE Pop();

   /// executes "return", popping new PC from the stack
   void Return();

   /// returns number of processed cycles since start of the processor
   unsigned int GetProcessedCycles() const { return m_uiElapsedCycles; }

   /// steals some cycles from the processor
   void StealCycles(unsigned int uiCycles)
   {
      m_uiElapsedCycles += uiCycles;
   }

   /// causes an interrupt in the processor
   void Interrupt(InterruptType enInterruptType);

private:
   /// loads 8-bit value from address
   BYTE Load(WORD wAddr);
   /// loads 16-bit little endian value from address 
   WORD Load16(WORD wAddr){ return LE2WORD(Load(wAddr), Load(wAddr+1)); }

   /// stores 8-bit value to address, taking memory mask into account
   void Store(WORD wAddr, BYTE bValue);

   /// fetches 16-bit address from PC using addressing mode
   WORD FetchAddress(AddressingMode enAddressingMode);

   /// loads register using addressing mode
   void LoadRegister(RegisterID enRegisterID, AddressingMode enAddressingMode);
   /// stores register using addressing mode
   void StoreRegister(RegisterID enRegisterID, AddressingMode enAddressingMode);
   /// transfers register value to another register
   void TransferRegister(RegisterID enRegSource, RegisterID enRegTarget);
   /// increases or decreases register or memory using addressing mode
   void IncDecRegister(RegisterID enRegister, AddressingMode enAddressingMode, bool bIncrease);
   /// compares register with memory location given by addressing mode
   void CompareRegister(RegisterID enRegisterID, AddressingMode enAddressingMode);
   /// branches using given opcode
   void Branch(BYTE bOpcode);
   /// carries out logical operation
   void LogicalOperation(LogicalOperation enLogicalOperation, AddressingMode enAddressingMode);
   /// carries out arithmetic operation
   void ArithmeticOperation(ArithmeticOperation enArithmeticOperation, AddressingMode enAddressingMode);
   /// carries out shift operation
   void ShiftOperation(ShiftOperation enShiftOperation, AddressingMode enAddressingMode);
   /// carries out bit operation
   void BitOperation(AddressingMode enAddressingMode);
   /// carries out set flag operation
   void SetFlagOperation(ProcessorFlag enFlag, bool bFlag);

private:
   /// program counter
   WORD m_wProgramCounter;
   /// register set of the processor
   BYTE m_abRegister[regLast+1];
   /// memory manager used for memory access
   MemoryManager& m_memoryManager;

   /// number of processed opcodes since processor start
   unsigned int m_uiProcessedOpcodes;
   /// number of elapsed cycles since processor start
   unsigned int m_uiElapsedCycles;

   /// processor callbacks
   std::set<IProcessorCallback*> m_setCallbacks;

   bool m_debugOutput;

   /// current opcode text; used in Step() to output currently processed opcode
   CString m_opcodeText;
};

} // namespace C64
