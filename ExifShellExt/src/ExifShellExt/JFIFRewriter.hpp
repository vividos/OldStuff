//
// ExifShellExt - Exif data shell extension
// Copyright (C) 2008 Michael Fink
//
/// \file JFIFRewriter.hpp JFIF (JPEG File Interchange Format) rewriter
//
#pragma once

// includes
#include <ulib/stream/IStream.hpp>

class JFIFRewriter
{
public:
   JFIFRewriter(Stream::IStream& streamIn, Stream::IStream& streamOut)
      :m_streamIn(streamIn), m_streamOut(streamOut)
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
   virtual void OnBlock(BYTE bMarker, WORD wLength);

protected:
   Stream::IStream& m_streamIn;
   Stream::IStream& m_streamOut;
};
