//
// ulib - a collection of useful classes
// Copyright (C) 2006-2014 Michael Fink
//
/// \file TextFileStream.hpp text file stream
//
#pragma once

// needed includes
#include <ulib/stream/FileStream.hpp>
#include <ulib/stream/TextStreamFilter.hpp>

namespace Stream
{

/// text file stream
class TextFileStream: public TextStreamFilter
{
public:
   /// ctor; opens or creates text file stream
   TextFileStream(LPCTSTR pszFilename,
      FileStream::EFileMode fileMode,
      FileStream::EFileAccess fileAccess,
      FileStream::EFileShare fileShare,
      ETextEncoding textEncoding = textEncodingNative,
      ELineEndingMode lineEndingMode = lineEndingCRLF)
      :TextStreamFilter(m_fileStream, textEncoding, lineEndingMode),
       m_fileStream(pszFilename, fileMode, fileAccess, fileShare)
   {
   }

   /// special unicode characters
   enum ESpecialChars
   {
      BOM = 0xfeff,  ///< byte order mark U+FEFF (zero width no-break space)
   };

   /// returns if the file was successfully opened
   bool IsOpen() const throw() { return m_fileStream.IsOpen(); }
   /// returns true when the file end is reached
   bool AtEndOfStream() const throw(){ return m_fileStream.AtEndOfStream(); }

private:
   /// file stream
   FileStream m_fileStream;
};

} // namespace Stream
