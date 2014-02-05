//
// ExifShellExt - Exif data shell extension
// Copyright (C) 2008 Michael Fink
//
/// \file ExifRewriter.cpp EXIF data rewriter for JFIF files
//

// includes
#include "stdafx.h"
#include "ExifRewriter.hpp"
#include <ulib/stream/EndianAwareFilter.hpp>

void ExifRewriter::OnBlock(BYTE bMarker, WORD wLength)
{
   if (bMarker == APP0)
   {
      // skip block
      JFIFRewriter::OnBlock(bMarker, wLength);
      return;
   }

   if (!m_bFirstBlock)
   {
      if (bMarker == APP1)
      {
         // oops, another APP1 block; skip this one
         m_streamIn.Seek(static_cast<LONGLONG>(wLength), Stream::IStream::seekCurrent);
      }
      else
      {
         // no APP1 block; just write it out
         JFIFRewriter::OnBlock(bMarker, wLength);
      }
      return;
   }

   // in this position, we have the first block, which could be an APP1 block

   // write given exif data
   m_streamOut.WriteByte(0xff);
   m_streamOut.WriteByte(APP1);

   Stream::EndianAwareFilter endianFilterOut(m_streamOut);
   endianFilterOut.Write16BE(static_cast<WORD>(m_vecData.size()+2));

   DWORD dwWritten = 0;
   m_streamOut.Write(&m_vecData[0], static_cast<DWORD>(m_vecData.size()), dwWritten);
   ATLASSERT(m_vecData.size() == dwWritten);

   m_bFirstBlock = false;

   if (bMarker != APP1)
   {
      // no APP1 block? then write it
      JFIFRewriter::OnBlock(bMarker, wLength);
   }
   else
   {
      // just skip over source APP1 block
      m_streamIn.Seek(static_cast<LONGLONG>(wLength), Stream::IStream::seekCurrent);
   }
}
