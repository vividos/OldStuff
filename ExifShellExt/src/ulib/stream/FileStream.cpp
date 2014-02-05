//
// ulib - a collection of useful classes
// Copyright (C) 2006-2014 Michael Fink
//
/// \file FileStream.cpp file based stream
//

// includes
#include "stdafx.h"
#include <ulib/stream/FileStream.hpp>
#include <ulib/stream/StreamException.hpp>
#include <ulib/win32/ErrorMessage.hpp>

using Stream::FileStream;

/// \note when a file on a floppy or cdrom drive is tried to open without a disc in the drive,
///       a message box appears asking for a disc in the drive. Use SetErrorMode with flag
///       SEM_FAILCRITICALERRORS to prevent this.
/// \exception StreamException thrown when file couldn't be opened
FileStream::FileStream(LPCTSTR pszFilename, EFileMode fileMode, EFileAccess fileAccess, EFileShare fileShare)
:m_fileAccess(fileAccess),
 m_bEOF(true)
{
   ATLASSERT(pszFilename != NULL);

#if defined(DEBUG) || defined(_DEBUG)
   if (fileMode == modeTruncate) // write access needed in modeTruncate
      ATLASSERT((fileAccess & FileStream::accessWrite) != 0);
#endif

   HANDLE hFile = CreateFile(pszFilename,
      static_cast<DWORD>(fileAccess), // desired access
      static_cast<DWORD>(fileShare), // share mode
      NULL, // security attributes
      static_cast<DWORD>(fileMode == modeAppend ? modeOpen : fileMode),
      FILE_ATTRIBUTE_NORMAL,
      NULL); // template file handle

   if (hFile == INVALID_HANDLE_VALUE)
      throw Stream::StreamException(Win32::ErrorMessage().Get() + pszFilename, __FILE__, __LINE__);

   m_spHandle = std::shared_ptr<void>(hFile, CloseHandle);

   m_bEOF = false;

   if (fileMode == modeAppend)
      Seek(0L, FileStream::seekEnd);
}

/// \exception StreamException thrown when reading fails
bool FileStream::Read(void* bBuffer, DWORD dwMaxBufferLength, DWORD& dwBytesRead)
{
   ATLASSERT(m_spHandle.get() != NULL);
   ATLASSERT(true == CanRead());

   dwBytesRead = 0;
   BOOL bRet = ::ReadFile(m_spHandle.get(), bBuffer, dwMaxBufferLength, &dwBytesRead, NULL);

   if (bRet == FALSE)
      throw Stream::StreamException(_T("Read: ") + Win32::ErrorMessage().Get(), __FILE__, __LINE__);

   return dwBytesRead != 0;
}

bool FileStream::AtEndOfStream() const throw()
{
   if (!IsOpen() || m_bEOF)
      return true;

   try
   {
      FileStream& rThis = const_cast<FileStream&>(*this);

      // find out current file pointer
      ULONGLONG ullCurrent = rThis.Seek(0LL, Stream::IStream::seekCurrent);
      ULONGLONG ullEnd = rThis.Seek(0LL, Stream::IStream::seekEnd);

      if (ullCurrent == ullEnd)
         return true;

      // not at EOF, so seek back
      rThis.Seek(static_cast<LONGLONG>(ullCurrent), Stream::IStream::seekBegin);
   }
   catch(const Stream::StreamException& )
   {
      // exception while seek: possibly on end of stream
      return true;
   }

   return false;
}

/// \exception StreamException thrown when writing fails
void FileStream::Write(const void* bData, DWORD dwLength, DWORD& dwBytesWritten)
{
   ATLASSERT(m_spHandle.get() != NULL);
   ATLASSERT(true == CanWrite());

   dwBytesWritten = 0;
   BOOL bRet = ::WriteFile(m_spHandle.get(), bData,
      dwLength, &dwBytesWritten, NULL);

   if (bRet == FALSE)
      throw Stream::StreamException(_T("Write: ") + Win32::ErrorMessage().Get(), __FILE__, __LINE__);
}

/// \exception StreamException thrown when setting file position fails
ULONGLONG FileStream::Seek(LONGLONG llOffset, ESeekOrigin origin)
{
   ATLASSERT(m_spHandle.get() != NULL);
   ATLASSERT(true == CanSeek());

   LARGE_INTEGER li;
   li.QuadPart = llOffset;

   li.LowPart = ::SetFilePointer(m_spHandle.get(),
      li.LowPart,
      &li.HighPart,
      static_cast<DWORD>(origin));

   // win32 may have returned error, or the low part just was 0xffffffff
   if (li.LowPart == INVALID_SET_FILE_POINTER)
   {
      // ... so also check last error
      DWORD dwLastError = GetLastError();
      if (dwLastError != ERROR_SUCCESS)
         throw Stream::StreamException(_T("Seek: ") + Win32::ErrorMessage().Get(), __FILE__, __LINE__);
   }

   return li.QuadPart;
}

/// \exception StreamException thrown when getting position fails
ULONGLONG FileStream::Position()
{
   return Seek(0L, seekCurrent);
}

/// \exception StreamException thrown when getting file size fails
ULONGLONG FileStream::Length()
{
   ATLASSERT(m_spHandle.get() != NULL);

#ifdef _WIN32_WCE
   // CE has no GetFileSizeEx, so we have to use GetFileSize
   LARGE_INTEGER lpFileSize; lpFileSize.QuadPart = 0;
   DWORD dwRet = ::GetFileSize(m_spHandle.get(), reinterpret_cast<ULONG*>(&lpFileSize.HighPart));
   if (dwRet == 0xffffffff)
   {
      DWORD dwError = GetLastError();

      if (dwError != NO_ERROR)
         throw Stream::StreamException(_T("Length: ") + Tools::CWin32ErrorMessage(dwError).GetMessage(), __FILE__, __LINE__);
   }
#else
   LARGE_INTEGER lpFileSize; lpFileSize.QuadPart = 0;
   BOOL bRet = ::GetFileSizeEx(m_spHandle.get(), &lpFileSize);

   if (bRet == FALSE)
      throw Stream::StreamException(_T("Length: ") + Win32::ErrorMessage().Get(), __FILE__, __LINE__);
#endif

   return lpFileSize.QuadPart;
}

/// \exception StreamException thrown when flushing the file fails
void FileStream::Flush()
{
   ATLASSERT(m_spHandle.get() != NULL);
   ATLASSERT(true == CanWrite());

   BOOL bRet = ::FlushFileBuffers(m_spHandle.get());

   if (bRet == FALSE)
      throw Stream::StreamException(_T("Flush: ") + Win32::ErrorMessage().Get(), __FILE__, __LINE__);
}

void FileStream::Close() throw()
{
   ATLASSERT(m_spHandle.get() != NULL);

   m_spHandle.reset();

   m_bEOF = true;
}
