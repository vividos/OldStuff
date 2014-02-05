//
// ExifShellExt - Exif data shell extension
// Copyright (C) 2008 Michael Fink
//
/// \file ExifRewriter.hpp EXIF data rewriter for JFIF files
//
#pragma once

// includes
#include "JFIFRewriter.hpp"
#include <vector>

class ExifRewriter: public JFIFRewriter
{
public:
   ExifRewriter(Stream::IStream& streamIn, Stream::IStream& streamOut, std::vector<BYTE>& vecData)
      :JFIFRewriter(streamIn, streamOut),
       m_vecData(vecData),
       m_bFirstBlock(true)
   {
   }

   virtual ~ExifRewriter() {}

   virtual void OnBlock(BYTE bMarker, WORD wLength) override;

private:
   bool m_bFirstBlock;
   std::vector<BYTE>& m_vecData;
};
