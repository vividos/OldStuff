//
// ulib - a collection of useful classes
// Copyright (C) 2006-2014 Michael Fink
//
/// \file TextStreamFilter.hpp text stream filter
//
#pragma once

// needed includes
#include <ulib/config/AutoLink.hpp>
#include <ulib/stream/IStream.hpp>
#include <ulib/stream/ITextStream.hpp>
#include <boost/noncopyable.hpp>

namespace Stream
{

/// text stream filter
class ULIB_DECLSPEC TextStreamFilter: public ITextStream, public boost::noncopyable
{
public:
   /// creates or opens text file stream
   TextStreamFilter(IStream& stream,
      ETextEncoding textEncoding = textEncodingNative,
      ELineEndingMode lineEndingMode = lineEndingNative);

   /// reads a single character
   virtual TCHAR ReadChar();

   /// reads a whole line
   virtual void ReadLine(CString& cszLine);

   /// writes text
   virtual void Write(const CString& cszText);

   /// writes endline character
   virtual void WriteEndline();

   /// returns underlying stream (const version)
   const IStream& Stream() const throw() { return m_stream; }

   /// returns underlying stream
   IStream& Stream() throw() { return m_stream; }

   /// returns true when stream can be read
   virtual bool CanRead() const throw(){ return m_stream.CanRead(); }

   /// returns true when stream can be written to
   virtual bool CanWrite() const throw(){ return m_stream.CanWrite(); }

   /// returns true when the stream end is reached
   virtual bool AtEndOfStream() const throw(){ return m_stream.AtEndOfStream(); }

   /// flushes out text stream
   virtual void Flush(){ m_stream.Flush(); }

private:
   /// puts back one character
   void PutBackChar(TCHAR ch);

   /// determines text encoding by reading bytes
   void DetermineTextEncoding();

private:
   /// stream to read from / write to
   IStream& m_stream;

   /// indicates if a character was put back
   bool m_bPutbackChar;

   /// the put back character
   TCHAR m_cPutbackChar;
};

} // namespace Stream
