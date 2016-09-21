//
// Emulator - Simple C64 emulator
// Copyright (C) 2003-2016 Michael Fink
//
/// \file Keyboard.hpp C64 keyboard implementation
//
#pragma once

// includes
#include "ICIAPortHandler.hpp"

namespace C64
{

/// Implementation of the C64 keyboard
/// \note the keyboard is wired to the CIA 2 of the original C64
class Keyboard: public ICIAPortHandler
{
public:
   /// ctor
   Keyboard();

   /// dtor
   ~Keyboard() throw()
   {
   }

   /// sets state for a key
   void SetKeyState(BYTE keyCode, bool keyState, bool shiftState);

private:
   /// calculates port A and port B bit indices from virtual key code and shift state
   bool CalcPortBitIndices(BYTE keyCode, bool shiftState, BYTE& pa, BYTE& pb);

   // virtual methods from ICIAPortHandler
   void SetDataPort(BYTE portNumber, BYTE value) throw() override;
   void ReadDataPort(BYTE portNumber, BYTE& value) const throw() override;

private:
   /// current key matrix (all set bits are currently pressed keys)
   BYTE m_keyMatrix[8];

   /// value that was written to port A (0 bits are the "interested bits")
   BYTE m_currentPortA;

   /// value that was written to port B (0 bits are the "interested bits")
   BYTE m_currentPortB;
};

} // namespace C64
