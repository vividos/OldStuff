//
// ExifShellExt - Exif data shell extension
// Copyright (C) 2008 Michael Fink
//
/// \file JFIFReader.cpp JFIF (JPEG File Interchange Format) reader
//

// includes
#include "stdafx.h"
#include "JFIFReader.hpp"
#include <ulib/Exception.hpp>
#include <ulib/stream/EndianAwareFilter.hpp>

void JFIFReader::Start()
{
   Stream::EndianAwareFilter endianFilterIn(m_streamIn);

   bool bStartOfStream = false;

   // read until EOF
   BYTE bMarker = SOI;
   while (!m_streamIn.AtEndOfStream() || bMarker != EOI)
   {
      BYTE bStartByte = m_streamIn.ReadByte();

      if (bStartByte != 0xff)
         throw Exception(_T("found invalid jfif start byte"), __FILE__, __LINE__);

      bMarker = m_streamIn.ReadByte();

      WORD wLength = 0;
      switch (bMarker)
      {
      case SOI:
         ATLTRACE(_T("JFIF Marker: SOI\n"));
         break;

      case EOI:
         break;

      case APP1:
         wLength = endianFilterIn.Read16BE();
         ATLTRACE(_T("JFIF Marker: APP1, length 0x%04x\n"), wLength);
         break;

      case DQT: // definition quantize tables
         wLength = endianFilterIn.Read16BE();
         ATLTRACE(_T("JFIF Marker: DQT, length 0x%04x\n"), wLength);
         break;

      case SOF0: // SOF 0: baseline DCT
         wLength = endianFilterIn.Read16BE();
         ATLTRACE(_T("JFIF Marker: SOF 0, length 0x%04x\n"), wLength);
         break;

      case DHT: // DHT: definition huffman tables
         wLength = endianFilterIn.Read16BE();
         ATLTRACE(_T("JFIF Marker: DHT, length 0x%04x\n"), wLength);
         break;

      case SOS:
         wLength = endianFilterIn.Read16BE();
         ATLTRACE(_T("JFIF Marker: SOS, length 0x%04x\n"), wLength);
         bStartOfStream = true;
         break;

      default:
         wLength = endianFilterIn.Read16BE();
         ATLTRACE(_T("JFIF Marker: 0x%02x, length 0x%04x\n"), bMarker, wLength);
         break;
      }

      if (bMarker != SOI && bMarker != EOI)
      {
         if (bStartOfStream)
         {
            // start of stream; read until 0xff 0xda arrives
//            OnStreamDataBegin(bStartByte, bMarker, wLength);

            // seek to end
            m_streamIn.Seek(0LL, Stream::IStream::seekEnd);

            bMarker = EOI; // end parsing with setting this marker
         }
         else
         {
            // skip over block
            ULONGLONG ullPos = m_streamIn.Position() + wLength - 2;
            if (wLength > 0)
            {
               OnBlock(bStartByte, bMarker, wLength);

               m_streamIn.Seek(static_cast<LONGLONG>(ullPos), Stream::IStream::seekBegin);
            }
         }
      }
   }
}
