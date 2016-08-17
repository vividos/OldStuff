//
// Emulator - Simple C64 emulator
// Copyright (C) 2003-2016 Michael Fink
//
/// \file TapeFile.cpp C64 Tape file (.t64) class
//

// includes
#include "StdAfx.h"
#include "TapeFile.hpp"
#include "Common.hpp"
#include <memory>

using C64::TapeFile;
using C64::TapeEntry;

TapeFile::TapeFile(LPCTSTR pszFilename)
:m_pszFilename(pszFilename)
{
   std::shared_ptr<FILE> fd =
      std::shared_ptr<FILE>(_tfopen(pszFilename, _T("rb")), fclose);

   ATLASSERT(fd.get() != NULL);

   BYTE abBuffer[32];
   fread(abBuffer, sizeof(BYTE), sizeof(abBuffer)/sizeof(BYTE), fd.get()); 
   fread(abBuffer, sizeof(BYTE), sizeof(abBuffer)/sizeof(BYTE), fd.get()); 

   WORD wEntries = LE2WORD(abBuffer[2], abBuffer[3]);

   for(WORD w = 0; w<wEntries; w++)
   {
      fread(abBuffer, sizeof(BYTE), sizeof(abBuffer)/sizeof(BYTE), fd.get()); 

      TapeEntry entry;

      entry.m_wLoadAddr = LE2WORD(abBuffer[2], abBuffer[3]);

      WORD wEndAddr = LE2WORD(abBuffer[4], abBuffer[5]);
      entry.m_wLength = wEndAddr - entry.m_wLoadAddr;

      entry.m_uiOffset = abBuffer[8];
      entry.m_uiOffset |= DWORD(abBuffer[9]) << 8;
      entry.m_uiOffset |= DWORD(abBuffer[10]) << 16;
      entry.m_uiOffset |= DWORD(abBuffer[11]) << 24;

      m_vecTapeEntries.push_back(entry);
   }
}

bool TapeFile::Load(unsigned int uiIndex, std::vector<BYTE>& vecMemory)
{
   std::shared_ptr<FILE> fd =
      std::shared_ptr<FILE>(_tfopen(m_pszFilename, _T("rb")), fclose);

   TapeEntry& entry = m_vecTapeEntries[uiIndex];

   fseek(fd.get(), entry.m_uiOffset, SEEK_SET);

   size_t iRead = fread(&vecMemory[entry.m_wLoadAddr], 1, entry.m_wLength, fd.get());
   ATLASSERT(iRead == entry.m_wLength);

   return iRead == entry.m_wLength;
}
