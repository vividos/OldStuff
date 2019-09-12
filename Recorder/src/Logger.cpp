//
// Recorder - a GPS logger app for Windows Mobile
// Copyright (C) 2006-2019 Michael Fink
//
/// \file Logger.cpp Logger implementation
//
#include "StdAfx.h"
#include "Logger.hpp"

void LogDebug(LPCTSTR msg, LPCTSTR category)
{
   CString text;
   text.Format(_T("[%s] %s\n"), category, msg);

   OutputDebugString(text);
}

FILE* s_logfile = NULL;

void LogInfo(LPCTSTR msg, LPCTSTR category)
{
   LogDebug(msg, category);

   if (s_logfile == NULL)
      s_logfile = fopen("\\Recorder.txt", "w");

   fprintf(s_logfile, "[%ls] %ls\n", category, msg);
   fflush(s_logfile);
}
