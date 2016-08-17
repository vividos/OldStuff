//
// Emulator - Simple C64 emulator
// Copyright (C) 2003-2016 Michael Fink
//
/// \file PC64File.cpp PC64 file loader
//

// includes
#include "StdAfx.h"
#include "PC64File.hpp"
#include "Common.hpp"
#include <memory>
#include <algorithm>

using C64::PC64File;

PC64File::PC64File(LPCTSTR pszFilename)
:m_bValid(false),
 m_enFileType(fileTypePrg),
 m_wPrgStart(0),
 m_pszFilename(pszFilename)
{
   std::shared_ptr<FILE> fd =
      std::shared_ptr<FILE>(_tfopen(pszFilename, _T("rb")), fclose);

   if (fd.get() == NULL)
      return;

   // read header
   BYTE abBuffer[26 + 2];
   fread(abBuffer, sizeof(BYTE), sizeof(abBuffer)/sizeof(BYTE), fd.get()); 

   if (0 != memcmp(abBuffer, "C64File\0", 8))
      return;

   // bytes 0x08 - 0x17 contain PETASCII filename

   // bytes 0x1a and 0x1b contains program start on p00 files
   m_wPrgStart = LE2WORD(abBuffer[0x1a], abBuffer[0x1b]);

   m_bValid = true;
}

bool PC64File::Load(std::vector<BYTE>& vecRam)
{
   std::shared_ptr<FILE> fd =
      std::shared_ptr<FILE>(_tfopen(m_pszFilename, _T("rb")), fclose);

   if (fd.get() == NULL)
      return false;

   // get length
   fseek(fd.get(), 0L, SEEK_END);
   long lLength = ftell(fd.get()) - 0x1c;

   fseek(fd.get(), 0x1c, SEEK_SET);

   size_t iSize = std::min<size_t>(lLength, 0x10000 - m_wPrgStart);

   size_t iRet = fread(&vecRam[m_wPrgStart], 1, iSize, fd.get());
   ATLASSERT(iRet == iSize);

   return iRet == iSize;
}
