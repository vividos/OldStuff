//
// ulib - a collection of useful classes
// Copyright (C) 2006-2014 Michael Fink
//
/// \file PrivilegeManager.cpp Privilege manager
//

// includes
#include "stdafx.h"

#ifndef _WIN32_WCE
#include <ulib/win32/PrivilegeManager.hpp>
#include <ulib/win32/ErrorMessage.hpp>
#include <vector>

using Win32::PrivilegeManager;

bool PrivilegeManager::Get(LPCTSTR pszPrivilegeName) const
{
   LUID luid;
   if (!::LookupPrivilegeValue(
      NULL,            // lookup privilege on local system
      pszPrivilegeName,// privilege to lookup
      &luid ) )        // receives LUID of privilege
   {
      DWORD dwError = GetLastError(); dwError;
      ATLTRACE(_T("LookupPrivilegeValue error: %u (%s)\n"), dwError, Win32::ErrorMessage(dwError).ToString().GetString());
      return false;
   }

   // find out length
   DWORD dwLength = 0;
   GetTokenInformation(m_hToken, TokenPrivileges, NULL, 0, &dwLength);

   // get privilege array
   if (dwLength == 0)
      return false;

   std::vector<BYTE> vecData(dwLength);

   TOKEN_PRIVILEGES& privileges = *reinterpret_cast<TOKEN_PRIVILEGES*>(&vecData[0]);
   GetTokenInformation(m_hToken, TokenPrivileges, &privileges, dwLength, &dwLength);

   // search for privilege
   for (DWORD dw=0; dw<privileges.PrivilegeCount; dw++)
   {
      if (0 == memcmp(&privileges.Privileges[dw].Luid, &luid, sizeof(luid)))
      {
         // found privilege
         //ATLTRACE(_T("found privilege %08x%08x, attr=%08x\n"),
         //   luid.HighPart, luid.LowPart, privileges.Privileges[dw].Attributes);

         return (privileges.Privileges[dw].Attributes & SE_PRIVILEGE_ENABLED) != 0 ||
            (privileges.Privileges[dw].Attributes & SE_PRIVILEGE_ENABLED_BY_DEFAULT) != 0;
      }
   }

   return false;
}

CString PrivilegeManager::GetDisplayName(LPCTSTR pszPrivilegeName) const
{
   // get string length
   DWORD dwBufferLength = 0;
   DWORD dwLanguageId = 0;
   ::LookupPrivilegeDisplayName(
      NULL, // local system
      pszPrivilegeName,
      NULL,
      &dwBufferLength,
      &dwLanguageId);

   CString cszName(_T(""));
   if (dwBufferLength > 0)
   {
      BOOL bRet = ::LookupPrivilegeDisplayName(
         NULL, // local system
         pszPrivilegeName,
         cszName.GetBuffer(dwBufferLength),
         &dwBufferLength,
         &dwLanguageId);

      cszName.ReleaseBuffer(bRet == FALSE ? 0 : dwBufferLength);
   }

   return cszName;
}

/// the process handle at least needs PROCESS_QUERY_INFORMATION rights to set
/// the privilege.
/// \param pszPrivilege name of privilege to enable/disable
/// \param bEnablePrivilege to enable or disable privilege
bool PrivilegeManager::Set(LPCTSTR pszPrivilege, bool bEnablePrivilege)
{
   LUID luid;
   if (!::LookupPrivilegeValue(
      NULL,            // lookup privilege on local system
      pszPrivilege,    // privilege to lookup
      &luid ) )        // receives LUID of privilege
   {
      DWORD dwError = GetLastError();
      CString cszText = Win32::ErrorMessage(dwError).ToString();
      ATLTRACE(_T("LookupPrivilegeValue error: %u (%s)\n"), dwError, cszText.GetString());
   }

   TOKEN_PRIVILEGES tp;

   tp.PrivilegeCount = 1;
   tp.Privileges[0].Luid = luid;
   tp.Privileges[0].Attributes = bEnablePrivilege ? SE_PRIVILEGE_ENABLED : 0; // 0 disables the privilege

   // Enable the privilege or disable all privileges.
   if (!::AdjustTokenPrivileges(
      m_hToken,
      FALSE,
      &tp,
      0,
      (PTOKEN_PRIVILEGES) NULL,
      (PDWORD) NULL) )
   {
      DWORD dwError = GetLastError();
      CString cszText = Win32::ErrorMessage(dwError).ToString();
      ATLTRACE(_T("AdjustTokenPrivileges error: %u (%s)\n"), dwError, cszText.GetString());
      return false;
   }

   return true;
}

#endif // _WIN32_WCE
