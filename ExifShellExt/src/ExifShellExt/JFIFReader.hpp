//
// ExifShellExt - Exif data shell extension
// Copyright (C) 2008 Michael Fink
//
/// \file JFIFReader.hpp JFIF (JPEG File Interchange Format) reader
//
#pragma once

// includes
#include <ulib/stream/IStream.hpp>

/// reader for files in JFIF format
class JFIFReader
{
public:
   JFIFReader(Stream::IStream& streamIn)
      :m_streamIn(streamIn)
   {
   }

   void Start();

   enum T_enJFIFBlockMarker
   {
      SOI  = 0xd8,
      EOI  = 0xd9,
      APP0 = 0xe0,
      APP1 = 0xe1,
      DQT  = 0xdb,
      SOF0 = 0xc0,
      DHT  = 0xc4,
      SOS  = 0xda,
   };

protected:
   virtual void OnBlock(BYTE bStartByte, BYTE bMarker, WORD wLength){ bStartByte; bMarker; wLength; }

protected:
   Stream::IStream& m_streamIn;
};
