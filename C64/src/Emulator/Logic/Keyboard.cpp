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

void Keyboard::SetDataPort(BYTE bValue) throw()
{
   bValue;
}

void Keyboard::SetDataDirection(BYTE bValue) throw()
{
   bValue;
}

bool Keyboard::ReadDataPort(BYTE& bValue) const throw()
{
   bValue;
   return true;
}
