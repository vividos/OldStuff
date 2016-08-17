//
// Emulator - Simple C64 emulator
// Copyright (C) 2003-2016 Michael Fink
//
/// \file PC64File.hpp PC64 file loader
//
#pragma once

// includes
#include <vector>

namespace C64
{

/// file type of PC64 file
enum FileType
{
   fileTypePrg=0, ///< program
   fileTypeSeq,   ///< sequential file
   fileTypeUsr,   ///< user file
   fileTypeRel,   ///< rel file
};

/// contains a PC64 format file, e.g. P00, S00, U00, R00
/// see http://www.unusedino.de/ec64/technical/formats/pc64.html
class PC64File
{
public:
   /// ctor; loads header infos from file
   PC64File(LPCTSTR pszFilename);
   /// dtor
   ~PC64File() throw() {}

   /// indicates if file is valid
   bool IsValid() const throw() { return m_bValid; }

   /// returns file type
   FileType GetFileType() const throw() { return m_enFileType; }

   /// loads file into RAM, at memory address stored in file
   bool Load(std::vector<BYTE>& vecRam);

private:
   bool m_bValid;          ///< valid flag
   FileType m_enFileType;  ///< file type
   WORD m_wPrgStart;       ///< program start memory address
   LPCTSTR m_pszFilename;  ///< filename
};

} // namespace C64
