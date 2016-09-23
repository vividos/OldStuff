//
// c64emu - C64 Emulator using SDL
// Copyright (C) 2006-2016 Michael Fink
//
/// \file EmulatorOptions.hpp Emulator command line options
//
#pragma once

// includes
#include "ProgramOptions.hpp"

/// options for command line application
class EmulatorOptions : public ProgramOptions
{
public:
   /// ctor
   EmulatorOptions();

   /// returns filename of file to load
   CString Filename() const { return m_filename; }

   /// returns index of entry to load in file; when negative, all entries are loaded
   int EntryIndex() const throw() { return m_entryIndex; }

   /// returns if window is initially shown fullscreen
   bool Fullscreen() const throw() { return m_fullscreen; }

   /// returns if debug mode is on
   bool DebugMode() const throw() { return m_debugMode; }

   /// returns if joystick NumPad emulation is enabled
   bool JoystickNumPadEmulation() const throw() { return m_joystickNumPadEmulation; }

   /// returns start program counter, or 0 when none was set
   WORD StartProgramCounter() const throw() { return m_startProgramCounter; }

private:
   /// filename of file to load
   CString m_filename;

   /// entry index in file to load; may also be negative
   int m_entryIndex;

   /// indicates if window is initially shown fullscreen
   bool m_fullscreen;

   /// indicates if debug mode is on
   bool m_debugMode;

   /// indicates if joystick NumPad emulation is enabled
   bool m_joystickNumPadEmulation;

   /// start program counter
   WORD m_startProgramCounter;
};
