//
// c64emu - C64 Emulator using SDL
// Copyright (C) 2006-2016 Michael Fink
//
/// \file EmulatorOptions.cpp Emulator command line options
//

// includes
#include "stdafx.h"
#include "EmulatorOptions.hpp"

EmulatorOptions::EmulatorOptions()
   :m_entryIndex(0),
   m_fullscreen(false),
   m_debugMode(false),
   m_joystickNumPadEmulation(false),
   m_startProgramCounter(0x0000)
{
   RegisterOutputHandler(&ProgramOptions::OutputConsole);
   RegisterHelpOption();

   RegisterOption(_T("l"), _T("load"), _T("loads specified tape (.t64) or program file (.p00)"), m_filename);

   RegisterOption(_T("e"), _T("entry"), _T("specifies an entry number when loading a tape (.t64) file; specify * to load all entries from a tape image"),
      [&](const CString& arg) -> bool
   {
      if (arg == "*")
         m_entryIndex = -1; // load all entries
      else
         m_entryIndex = _ttoi(arg);

      return true;
   });

   RegisterOption(_T("f"), _T("fullscreen"),
      _T("shows the emulator in fullscreen on start (toggle with Alt+Enter)"),
      0,
      [&](auto dummy) -> bool
   {
      m_fullscreen = true;
      return true;
   });

   RegisterOption(_T("j"), _T("joystick"),
      _T("enables port 2 joystick emulation via NumPad and Right-Ctrl keys"),
      0,
      [&](auto dummy) -> bool
   {
      m_joystickNumPadEmulation = true;
      return true;
   });

   RegisterOption(_T("s"), _T("sys"), _T("sets start program counter, in decimal (e.g. 2064) or hex ($0820, 0x0820)"),
      [&](const CString& arg) -> bool
   {
      if (arg.Find(_T("$")) != -1)
         m_startProgramCounter = static_cast<WORD>(_tcstoul(arg.Mid(1), nullptr, 16));
      else
         if (arg.Find(_T("0x")) != -1)
            m_startProgramCounter = static_cast<WORD>(_tcstoul(arg.Mid(2), nullptr, 16));
         else
            m_startProgramCounter = static_cast<WORD>(_ttoi(arg));

      return true;
   });

   RegisterOption(_T("d"), _T("debug"),
      _T("runs emulator in debug mode, showing VIC and CPU status infos"),
      0,
      [&](auto dummy) -> bool
   {
      m_debugMode = true;
      return true;
   });
}
