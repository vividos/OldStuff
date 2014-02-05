//
// ulib - a collection of useful classes
// Copyright (C) 2006-2014 Michael Fink
//
/// \file IStream.hpp stream interface
//
#pragma once

/// \brief stream related classes
namespace Stream
{

/// stream interface
class IStream
{
public:
   /// origin for Seek() operations
   enum ESeekOrigin
   {
      seekBegin = 0,
      seekCurrent = 1,
      seekEnd = 2,
   };

   /// dtor
   virtual ~IStream() throw() {}

   // stream capabilities

   /// returns true when stream can be read
   virtual bool CanRead() const throw() = 0;
   /// returns true when stream can be written to
   virtual bool CanWrite() const throw() = 0;
   /// returns true when seek operations are possible in the stream
   virtual bool CanSeek() const throw() = 0;

   // read support

   /// reads amount of data into given buffer; returns if stream is at its end
   virtual bool Read(void* bBuffer, DWORD dwMaxBufferLength, DWORD& dwBytesRead) = 0;
   /// reads one byte
   virtual BYTE ReadByte();

   /// returns true when the stream end is reached
   virtual bool AtEndOfStream() const throw() = 0;

   // write support

   /// writes out given data buffer
   virtual void Write(const void* bData, DWORD dwLength, DWORD& dwBytesWritten) = 0;
   /// writes out single byte
   virtual void WriteByte(BYTE b);

   // seek support

   /// seeks to given position, regarding given origin
   virtual ULONGLONG Seek(LONGLONG llOffset, ESeekOrigin origin) = 0;
   /// returns current position in stream
   virtual ULONGLONG Position() = 0;
   /// returns length of stream
   virtual ULONGLONG Length() = 0;

   /// flushes data
   virtual void Flush() = 0;
   /// closes stream
   virtual void Close() = 0;
};

inline BYTE IStream::ReadByte()
{
   BYTE b;
   DWORD dwBytesRead = 0;
   ATLVERIFY(true == Read(&b, 1, dwBytesRead) && 1 == dwBytesRead);
   return b;
}

inline void IStream::WriteByte(BYTE b)
{
   DWORD dwBytesWritten;
   Write(&b, 1, dwBytesWritten);
   ATLVERIFY(1 == dwBytesWritten);
}

} // namespace Stream
