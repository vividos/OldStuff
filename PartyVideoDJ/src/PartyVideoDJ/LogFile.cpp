//
// PartyVideoDJ - YouTube crossfading app
// Copyright (C) 2008-2017 Michael Fink
//
/// \file LogFile.cpp Log file for logging played titles
//
#include "StdAfx.h"
#include "LogFile.hpp"
#include <ulib/DateTime.hpp>

CString GetLogfileFilename()
{
   CString cszFilename;
   GetModuleFileName(NULL, cszFilename.GetBuffer(MAX_PATH), MAX_PATH);
   cszFilename.ReleaseBuffer();

   int iPos = cszFilename.ReverseFind(_T('\\'));
   cszFilename = cszFilename.Left(iPos + 1);
   cszFilename += _T("logbook.txt");
   return cszFilename;
}

LogFile::LogFile()
{
   try
   {
      m_spStream.reset(new Stream::TextFileStream(GetLogfileFilename(),
         Stream::FileStream::modeOpenOrCreate,
         Stream::FileStream::accessWrite,
         Stream::FileStream::shareRead));

      if (m_spStream != NULL)
         m_spStream->Stream().Seek(0LL, Stream::IStream::seekEnd);
   }
   catch (...)
   {
   }

   LogText(_T("=== Party Video DJ started ==="), true);
}

LogFile::~LogFile()
{
   if (m_spStream != NULL)
   {
      m_spStream->Flush();
      m_spStream.reset();
   }
}

void LogFile::LogSong(const CString& cszTitle)
{
   LogText(cszTitle, false);
}

void LogFile::LogText(const CString& cszText, bool bLongDate)
{
   if (m_spStream == NULL)
      return;

   DateTime dt = DateTime::Now();

   CString cszTemp;
   cszTemp.Format(_T("[%s] %s"),
      dt.Format(bLongDate ? _T("%Y-%m-%d %H:%M:%S") : _T("%H:%M:%S")).GetString(),
      cszText.GetString());

   m_spStream->Write(cszTemp);
   m_spStream->WriteEndline();
   m_spStream->Flush();
}
