//
// c64emu - C64 Emulator using SDL
// Copyright (C) 2006-2016 Michael Fink
//
/// \file android_main.cpp Android main function
//

// includes
#include "stdafx.h"
#include "EmulatorWindow.hpp"

extern "C" void Java_org_libsdl_app_SDLActivity_nativeInit();

/// main function
int main(int argc, char *argv[])
{
   void* p = (void*)&Java_org_libsdl_app_SDLActivity_nativeInit;

   _ftprintf(stdout, _T("C64 Emulator\n\n"));

   C64::Machine machine;
   EmulatorWindow window(machine);

   window.Run();

   return 0;
}
