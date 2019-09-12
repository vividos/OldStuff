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
   m_outputStream = _wfopen(cszFilename, _T("wb"));

   // seek to end
   fseek(m_outputStream, 0, SEEK_END);

   Receiver().SetRawOutputStream(m_outputStream);
}

void CApplication::CloseNMEALog()
{
   // detach output from receiver
   Receiver().SetRawOutputStream();

   // close log file
   fclose(m_outputStream);
   m_outputStream = NULL;
}
