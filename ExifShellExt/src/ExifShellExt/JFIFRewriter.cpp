//
// ExifShellExt - Exif data shell extension
// Copyright (C) 2008 Michael Fink
//
/// \file JFIFRewriter.cpp JFIF (JPEG File Interchange Format) rewriter
//

// includes
#include "StdAfx.h"
#include "JFIFRewriter.hpp"
#include <ulib/stream/EndianAwareFilter.hpp>
#include <ulib/Exception.hpp>
#include <algorithm>

const BYTE c_bStartByte = 0xff;

void JFIFRewriter::Start()
{
   Stream::EndianAwareFilter endianFilterIn(m_streamIn);
   Stream::EndianAwareFilter endianFilterOut(m_streamOut);

   bool bStartOfStream = false;

   // read until EOF
   BYTE bMarker = SOI;
   while (!m_streamIn.AtEndOfStream() || bMarker != EOI)
   {
      BYTE bStartByte = m_streamIn.ReadByte();

      if (bStartByte != c_bStartByte)
         throw Exception(_T("found invalid jfif start byte"), __FILE__, __LINE__);

      bMarker = m_streamIn.ReadByte();

      WORD wLength = 0;
      switch(bMarker)
      {
      case SOI:
         ATLTRACE(_T("JFIF Marker: SOI\n"));
         break;

      case EOI:
         break;

      case APP0:
         wLength = endianFilterIn.Read16BE();
         ATLTRACE(_T("JFIF Marker: APP0, length 0x%04x\n"), wLength);
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

      if (bMarker == SOI || bMarker == EOI)
      {
         m_streamOut.WriteByte(bStartByte);
         m_streamOut.WriteByte(bMarker);
      }

      if (bMarker != SOI && bMarker != EOI)
      {
         if (bStartOfStream)
         {
            m_streamOut.WriteByte(bStartByte);
            m_streamOut.WriteByte(bMarker);

            endianFilterOut.Write16BE(wLength);

            // start of stream; read until 0xff 0xda arrives
            BYTE abBuffer[1024];
            DWORD dwRead = 0, dwWritten = 0;
            while (!m_streamIn.AtEndOfStream())
            {
               m_streamIn.Read(abBuffer, sizeof(abBuffer), dwRead);
               if (dwRead > 0)
               {
                  m_streamOut.Write(abBuffer, dwRead, dwWritten);
                  ATLASSERT(dwRead == dwWritten);
               }
            }

            bMarker = EOI;
         }
         else
         {
            if (wLength > 2)
               OnBlock(bMarker, wLength-2);
            else
            {
               m_streamOut.WriteByte(bStartByte);
               m_streamOut.WriteByte(bMarker);
               endianFilterOut.Write16BE(wLength);
            }
         }
      }
   }
}

void JFIFRewriter::OnBlock(BYTE bMarker, WORD wLength)
{
   // write block header
   m_streamOut.WriteByte(c_bStartByte);
   m_streamOut.WriteByte(bMarker);

   Stream::EndianAwareFilter endianFilterOut(m_streamOut);
   endianFilterOut.Write16BE(wLength+2);

   // copy over bytes
   BYTE abBuffer[1024];
   DWORD dwRead = 0, dwWritten = 0;
   DWORD dwLength = wLength;

   while (!m_streamIn.AtEndOfStream() && dwLength > 0)
   {
      DWORD dwSize = std::min(static_cast<DWORD>(sizeof(abBuffer)), dwLength);

      m_streamIn.Read(abBuffer, dwSize, dwRead);

      dwLength -= dwRead;

      if (dwRead > 0)
      {
         m_streamOut.Write(abBuffer, dwRead, dwWritten);
         ATLASSERT(dwRead == dwWritten);
      }
   }
}
