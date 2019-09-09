//
// Recorder - a GPS logger app for Windows Mobile
// Copyright (C) 2006-2019 Michael Fink
//
/// \file Application.cpp Application object
//
#include "stdafx.h"
#include "Application.hpp"

CApplication::CApplication() throw()
{
   LOG_INFO(_T("*** Recorder application started"), Logging::App);
}

CApplication::~CApplication() throw()
{
   LOG_INFO(_T("*** Recorder application stopped"), Logging::App);
}

void CApplication::AppendNMEALog(const CString& cszFilename)
{
   // open or create output file
   boost::shared_ptr<Stream::CTextFileStream> spTextStream(
      new Stream::CTextFileStream(cszFilename,
         Stream::CFileStream::modeOpenOrCreate,
         Stream::CFileStream::accessWrite,
         Stream::CFileStream::shareRead,
         Stream::ITextStream::textEncodingAnsi,
         Stream::ITextStream::lineEndingCRLF));

   // seek to end
   spTextStream->Seek(0LL, Stream::IStream::seekEnd);

   m_spNMEALogTextStream = spTextStream;

   Receiver().SetRawOutputStream(m_spNMEALogTextStream);
}

void CApplication::CloseNMEALog()
{
   // detach output from receiver
   Receiver().SetRawOutputStream();

   // close log file
   m_spNMEALogTextStream->Flush();
   m_spNMEALogTextStream.reset();
}
