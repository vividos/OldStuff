//
// ulib - a collection of useful classes
// Copyright (C) 2006-2014 Michael Fink
//
/// \file ProcessInfo.cpp Process info
//

// includes
#include "stdafx.h"

#ifndef _WIN32_WCE
#include <ulib/win32/ProcessInfo.hpp>
#include <aclapi.h>
#include <vector>

bool Win32::ProcessInfo::GetUserAndDomainName(CString& cszUsername, CString& cszRefDomainName)
{
   HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, m_dwProcessId);
   if (hProcess == NULL)
      return false;

   bool bRet = false;
   HANDLE hProcessToken = NULL;
   if (TRUE == OpenProcessToken(hProcess, TOKEN_QUERY, &hProcessToken))
   {
      // get length
      DWORD nLength = 0;
      GetTokenInformation(hProcessToken, TokenUser, NULL, 0, &nLength);

      std::vector<BYTE> vecData(nLength);
      TOKEN_USER* pTokenUser = (TOKEN_USER*)&vecData[0];

      if (TRUE == GetTokenInformation(hProcessToken, TokenUser, pTokenUser, nLength, &nLength))
      {
         DWORD cchName = 0, cchReferencedDomainName = 0;
         LookupAccountSid(NULL, pTokenUser->User.Sid, NULL, &cchName, NULL, &cchReferencedDomainName, NULL);

         // for some unknown reason the direct use of the output parameters don't work, so use
         // local objects here and assign later
         CString cszUsernameL, cszRefDomainNameL;
         SID_NAME_USE eUse;
         LookupAccountSid(NULL, pTokenUser->User.Sid,
            cszUsernameL.GetBuffer(cchName), &cchName,
            cszRefDomainNameL.GetBuffer(cchReferencedDomainName), &cchReferencedDomainName,
            &eUse);

         cszUsernameL.ReleaseBuffer();
         cszRefDomainNameL.ReleaseBuffer();

         cszUsername = cszUsernameL;
         cszRefDomainName = cszRefDomainNameL;
         bRet = true;
      }

      CloseHandle(hProcessToken);
   }

   return bRet;
}

#endif // _WIN32_WCE
