//
// Emulator - Simple C64 emulator
// Copyright (C) 2003-2016 Michael Fink
//
/// \file Keyboard.cpp C64 keyboard implementation
//

// includes
#include "StdAfx.h"
#include "Keyboard.hpp"

using C64::Keyboard;

Keyboard::Keyboard()
{
}

void Keyboard::SetDataPort(BYTE portNumber, BYTE bValue) throw()
{
   bValue;
}

void Keyboard::SetDataDirection(BYTE portNumber, BYTE bValue) throw()
{
   bValue;
}

void Keyboard::ReadDataPort(BYTE portNumber, BYTE& bValue) const throw()
{
   bValue;
   return true;
}
