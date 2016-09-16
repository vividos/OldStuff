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
/// \todo implement
class Keyboard: public ICIAPortHandler
{
public:
   /// ctor
   Keyboard();

   /// dtor
   ~Keyboard() throw()
   {
   }

private:
   // virtual methods from ICIAPortHandler
   void SetDataPort(BYTE portNumber, BYTE value) throw() override;
   void SetDataDirection(BYTE portNumber, BYTE value) throw() override;
   void ReadDataPort(BYTE portNumber, BYTE& value) const throw() override;
};

} // namespace C64
