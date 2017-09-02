//
// PartyVideoDJ - YouTube crossfading app
// Copyright (C) 2008-2017 Michael Fink
//
/// \file LogFile.hpp Log file for logging played titles
//
#pragma once

#include <ulib/stream/TextFileStream.hpp>

class LogFile
{
public:
   LogFile();
   ~LogFile();

   void LogSong(const CString& cszTitle);

private:
   void LogText(const CString& cszText, bool bLongDate);

private:
   std::unique_ptr<Stream::TextFileStream> m_spStream;
};
