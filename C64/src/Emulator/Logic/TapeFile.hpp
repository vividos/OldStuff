//
// Emulator - Simple C64 emulator
// Copyright (C) 2003-2016 Michael Fink
//
/// \file TapeFile.hpp C64 Tape file (.t64) class
//
#pragma once

// includes
#include <vector>

namespace C64
{

/// a single tape file entry
struct TapeEntry
{
   unsigned int m_uiOffset;   ///< file offset
   WORD m_wLoadAddr;          ///< load memory address
   WORD m_wLength;            ///< length of data
};

/// Tape file (.t64) loader
class TapeFile
{
public:
   /// ctor; loads tape entries
   TapeFile(LPCTSTR pszFilename);
   /// dtor
   ~TapeFile() throw() {}

   /// loads a tape file entry to RAM, using stored load memory address
   bool Load(unsigned int uiIndex, std::vector<BYTE>& vecMemory);

private:
   /// tape file name
   LPCTSTR m_pszFilename;

   /// list of tape entries
   std::vector<TapeEntry> m_vecTapeEntries;
};

} // namespace C64
