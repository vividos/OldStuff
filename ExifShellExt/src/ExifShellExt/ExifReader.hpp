//
// ExifShellExt - Exif data shell extension
// Copyright (C) 2008 Michael Fink
//
/// \file ExifReader.hpp Exif data reader for JFIF files
//
#pragma once

// includes
#include "JFIFReader.hpp"
#include <vector>

class ExifReader: public JFIFReader
{
public:
   ExifReader(Stream::IStream& streamIn)
      :JFIFReader(streamIn)
   {
   }

   const std::vector<BYTE>& GetData() const throw() { return m_vecExifData; }

private:
   virtual void OnBlock(BYTE bStartByte, BYTE bMarker, WORD wLength) override
   {
      bStartByte;
      if (bMarker == APP1)
      {
         m_vecExifData.resize(wLength-2);

         DWORD dwLengthRead = 0;
         m_streamIn.Read(&m_vecExifData[0], wLength-2, dwLengthRead);

         // couldn't read all? clear exif data; isn't complete
         if (dwLengthRead != static_cast<DWORD>(wLength-2))
            m_vecExifData.clear();
      }
   }

private:
   std::vector<BYTE> m_vecExifData;
};
