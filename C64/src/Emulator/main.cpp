//
// c64emu - C64 Emulator using SDL
// Copyright (C) 2006-2016 Michael Fink
//
/// \file main.cpp main function
//

// includes
#include "stdafx.h"
#include "EmulatorOptions.hpp"
#include "EmulatorWindow.hpp"

/// main function
int _tmain(int argc, _TCHAR* argv[])
{
   _ftprintf(stdout, _T("C64 Emulator\n\n"));

   EmulatorOptions options;
   options.Parse(argc, argv);

   if (options.IsSelectedHelpOption())
      return 0;

   C64::Machine machine;
   EmulatorWindow window(machine);

   if (options.DebugMode())
   {
      machine.GetVideoInterfaceController().SetShowDebugInfo(true);
   }

   if (options.StartProgramCounter() != 0)
   {
      window.SetStartProgramCounter(options.StartProgramCounter());
   }

   window.Load(options.Filename(), options.EntryIndex());

   window.Run();

   return 0;
}
