//
// c64emu - C64 Emulator using SDL
// Copyright (C) 2006-2016 Michael Fink
//
/// \file main.cpp main function
//

// includes
#include "stdafx.h"
#include "EmulatorWindow.hpp"

/// main function
int _tmain(int argc, _TCHAR* argv[])
{
   C64::Machine machine;
   EmulatorWindow window(machine);

   if (argc != 2)
   {
      _ftprintf(stderr, _T("Syntax:\n   Emulator.exe {.t64-or-.prg-or-.p00-file}"));
      return 1;
   }

   window.Load(argv[1]);

   window.Run();

   return 0;
}
