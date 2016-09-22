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

   /// returns if debug mode is on
   bool DebugMode() const throw() { return m_debugMode; }

private:
   /// filename of file to load
   CString m_filename;

   /// entry index in file to load; may also be negative
   int m_entryIndex;

   /// indicates if debug mode is on
   bool m_debugMode;
};
