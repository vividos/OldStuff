//
// ulib - a collection of useful classes
// Copyright (C) 2006-2014 Michael Fink
//
/// \file ErrorMessage.hpp Win32 error message
//
#pragma once

#ifdef __ANDROID__
#include <cerrno>
#endif

namespace Win32
{

/// \brief Win32 API error message
/// \details Formats a readable error message from an error value obtained by
/// GetLastError() or other means.
class ErrorMessage
{
public:
   /// ctor; takes a win32 error code
   ErrorMessage(DWORD dwError = ErrorMessage::LastError())
      :m_dwError(dwError)
   {
   }

   /// Returns formatted error message
   CString Get()
   {
#ifdef WIN32
      LPVOID lpMsgBuf = NULL;
      ::FormatMessage(
         FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
         NULL,
         m_dwError,
         MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // default language
         reinterpret_cast<LPTSTR>(&lpMsgBuf), 0, NULL);

      CString cszErrorMessage;
      if (lpMsgBuf)
      {
         cszErrorMessage = reinterpret_cast<LPTSTR>(lpMsgBuf);
         LocalFree(lpMsgBuf);
      }

      cszErrorMessage.TrimRight(_T("\r\n"));
#endif

#ifdef __ANDROID__
      CString cszErrorMessage;

      cszErrorMessage.Format(_T("Error %i (%hs)"),
         static_cast<int>(m_dwError),
         strerror(static_cast<int>(m_dwError)));
#endif

      return cszErrorMessage;
   }

   static DWORD LastError() throw()
   {
#ifdef WIN32
      return GetLastError();
#endif

#ifdef __ANDROID__
      return static_cast<DWORD>(errno);
#endif
}

private:
   /// error code
   DWORD m_dwError;
};

} // namespace Win32
