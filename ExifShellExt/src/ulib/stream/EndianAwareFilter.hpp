//
// ulib - a collection of useful classes
// Copyright (C) 2006-2014 Michael Fink
//
/// \file EndianAwareFilter.hpp filter for reading/writing little and big endian values
//
#pragma once

// needed includes
#include <ulib/stream/IStream.hpp>

namespace Stream
{

/// stream filter for reading/writing endian aware WORD and DWORD values
class EndianAwareFilter
{
public:
   /// ctor
   EndianAwareFilter(IStream& stream) throw()
      :m_stream(stream)
   {
   }

   /// reads 16-bit word, little endian format (or host byte order)
   WORD Read16LE()
   {
      ATLASSERT(true == m_stream.CanRead());

      WORD w = m_stream.ReadByte(); // low-byte
      w |= static_cast<WORD>(m_stream.ReadByte()) << 8; // high-byte
      return w;
   }

   /// reads 16-bit word, big endian format (or network byte order)
   WORD Read16BE()
   {
      ATLASSERT(true == m_stream.CanRead());

      WORD w = static_cast<WORD>(m_stream.ReadByte()) << 8; // high-byte
      w |= m_stream.ReadByte(); // low-byte
      return w;
   }

   /// reads 32-bit word, little endian format (or host byte order)
   DWORD Read32LE()
   {
      ATLASSERT(true == m_stream.CanRead());

      DWORD dw = Read16LE(); // low-word
      dw |= static_cast<DWORD>(Read16LE()) << 16; // high-word
      return dw;
   }

   /// reads 32-bit word, big endian format (or network byte order)
   DWORD Read32BE()
   {
      ATLASSERT(true == m_stream.CanRead());

      DWORD dw = static_cast<DWORD>(Read16BE()) << 16; // low-word
      dw |= Read16BE(); // high-word
      return dw;
   }

   /// writes 16-bit word, little endian format (or host byte order)
   void Write16LE(WORD w)
   {
      ATLASSERT(true == m_stream.CanWrite());

      m_stream.WriteByte(static_cast<BYTE>(w & 0xff)); // low-byte
      m_stream.WriteByte(static_cast<BYTE>((w >> 8) & 0xff)); // high-byte
   }

   /// writes 16-bit word, big endian format (or network byte order)
   void Write16BE(WORD w)
   {
      ATLASSERT(true == m_stream.CanWrite());

      m_stream.WriteByte(static_cast<BYTE>((w >> 8) & 0xff)); // high-byte
      m_stream.WriteByte(static_cast<BYTE>(w & 0xff)); // low-byte
   }

   /// writes 32-bit word, little endian format (or host byte order)
   void Write32LE(DWORD dw)
   {
      ATLASSERT(true == m_stream.CanWrite());

      Write16LE(static_cast<WORD>(dw & 0xffff)); // low-word
      Write16LE(static_cast<WORD>((dw >> 16) & 0xffff)); // high-word
   }

   /// writes 32-bit word, big endian format (or network byte order)
   void Write32BE(DWORD dw)
   {
      ATLASSERT(true == m_stream.CanWrite());

      Write16BE(static_cast<WORD>((dw >> 16) & 0xffff)); // high-word
      Write16BE(static_cast<WORD>(dw & 0xffff)); // low-word
   }

private:
   /// stream to use
   IStream& m_stream;
};

} // namespace Stream
