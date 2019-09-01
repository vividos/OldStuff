#include "stdafx.h"
#include "Application.hpp"
#include <ulib/log/Layout.hpp>
#include <ulib/log/Appender.hpp>
#include <ulib/log/TextStreamAppender.hpp>
#include <ulib/stream/TextFileStream.hpp>

CApplication::CApplication() throw()
{
   // create appender
   Log::AppenderPtr spAppender1 = Log::AppenderPtr(new Log::OutputDebugStringAppender);

   boost::shared_ptr<Stream::CTextFileStream> spTextStream(new Stream::CTextFileStream(_T("\\Recorder.txt"),
         Stream::CFileStream::modeOpenOrCreate,
         Stream::CFileStream::accessWrite,
         Stream::CFileStream::shareRead,
         Stream::ITextStream::textEncodingAnsi,
         Stream::ITextStream::lineEndingCRLF));

   spTextStream->Seek(0LL, Stream::IStream::seekEnd);

   boost::shared_ptr<Stream::ITextStream> spLogStream = spTextStream;

   Log::AppenderPtr spAppender2 = Log::AppenderPtr(new Log::TextStreamAppender(spLogStream));

   // add layout to appender
   Log::LayoutPtr spLayout = Log::LayoutPtr(new Log::PatternLayout(_T("[%d] %p (%c) - %m")));
   spAppender1->Layout(spLayout);
   spAppender2->Layout(spLayout);

   // set as appender for root logger
   Log::Logger::GetRootLogger()->AddAppender(spAppender1);
   Log::Logger::GetRootLogger()->AddAppender(spAppender2);

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
