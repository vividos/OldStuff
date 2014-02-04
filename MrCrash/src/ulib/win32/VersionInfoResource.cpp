//
// ulib - a collection of useful classes
// Copyright (C) 2004-2014 Michael Fink
//
/// \file VersionInfoResource.cpp version info resource class
//

// includes
#include "stdafx.h"
#include <ulib/win32/VersionInfoResource.hpp>
#include <winbase.h> // for VOS_* constants
#include <winver.h> // for VOS_* constants, when not in winbase.h

#ifndef VOS_WINDOWSCE
#  define VOS_WINDOWSCE 0x00050000L
#endif

#ifndef _WIN32_WCE
#  pragma comment(lib, "version.lib")
#endif

using Win32::FixedFileInfo;
using Win32::VersionInfoResource;
using Win32::LANGANDCODEPAGE;

//
// FixedFileInfo
//

CString FixedFileInfo::GetFileVersion() const
{
   CString cszText;
   cszText.Format(_T("%u.%u.%u.%u"),
      HIWORD(dwFileVersionMS), LOWORD(dwFileVersionMS),
      HIWORD(dwFileVersionLS), LOWORD(dwFileVersionLS));
   return cszText;
}

CString FixedFileInfo::GetProductVersion() const
{
   CString cszText;
   cszText.Format(_T("%u.%u.%u.%u"),
      HIWORD(dwProductVersionMS), LOWORD(dwProductVersionMS),
      HIWORD(dwProductVersionLS), LOWORD(dwProductVersionLS));
   return cszText;
}

CString FixedFileInfo::GetFileOS() const
{
   CString cszText;

   switch(dwFileOS & 0xFFFF0000)
   {
   case VOS_DOS: // The file was designed for MS-DOS.  
      cszText = _T("VOS_DOS"); break;
   case VOS_OS216: // The file was designed for 16-bit OS/2.  
      cszText = _T("VOS_OS216"); break;
   case VOS_OS232: // The file was designed for 32-bit OS/2.  
      cszText = _T("VOS_OS232"); break;
   case VOS_NT: // The file was designed for Windows NT and Windows 2000.  
      cszText = _T("VOS_NT"); break;
   case VOS_WINDOWSCE:
      cszText = _T("VOS_WINDOWSCE"); break;
   case VOS_UNKNOWN:
      cszText = _T("VOS_UNKNOWN"); break;
   default:
      cszText = _T("unknown"); break;
   }

   switch(dwFileOS & 0x0000FFFF)
   {
   case VOS__WINDOWS16: // The file was designed for 16-bit Windows.  
      cszText += _T(" | VOS__WINDOWS16"); break;
   case VOS__PM16: // The file was designed for 16-bit Presentation Manager.  
      cszText += _T(" | VOS__PM16"); break;
   case VOS__PM32: // The file was designed for 32-bit Presentation Manager.  
      cszText += _T(" | VOS__PM32"); break;
   case VOS__WINDOWS32: // The file was designed for the Win32 API.  
      cszText += _T(" | VOS__WINDOWS32"); break;
   }

   return cszText;
}

CString FixedFileInfo::GetFileType() const
{
   CString cszText;
   switch(dwFileType)
   {
   case VFT_UNKNOWN:
      cszText = _T("VFT_UNKNOWN"); break;
   case VFT_APP:
      cszText = _T("VFT_APP"); break;
   case VFT_DLL:
      cszText = _T("VFT_DLL"); break;
   case VFT_DRV:
      cszText = _T("VFT_DRV"); break;
   case VFT_FONT:
      cszText = _T("VFT_FONT"); break;
   case VFT_VXD:
      cszText = _T("VFT_VXD"); break;
   case VFT_STATIC_LIB:
      cszText = _T("VFT_STATIC_LIB"); break;
   default:
      cszText = _T("unknown"); break;
   }
   return cszText;
}

//
// VersionInfoResource
//

VersionInfoResource::VersionInfoResource(LPCTSTR pszFilename)
{
   LPTSTR pszFilename2 = const_cast<LPTSTR>(pszFilename);

   // find out length
   DWORD dwDummy = 0;
   DWORD dwLen = ::GetFileVersionInfoSize(pszFilename2, &dwDummy);

   if (dwLen > 0)
   {
      m_vecVersionInfoData.resize(dwLen);

      // get version info
      ::GetFileVersionInfo(pszFilename2, 0, dwLen, &m_vecVersionInfoData[0]);
   }
}

LPVOID VersionInfoResource::QueryValue(LPCTSTR pszSubBlock, UINT* puiSize)
{
   ATLASSERT(IsAvail());

   LPTSTR pszSubBlock2 = const_cast<LPTSTR>(pszSubBlock);

   LPVOID pData = NULL;
   UINT uiSize = 0;

   BOOL bRet = VerQueryValue(&m_vecVersionInfoData[0],
      pszSubBlock2,
      &pData,
      puiSize != NULL ? puiSize : &uiSize);

   return bRet == FALSE ? NULL : pData;
}

void VersionInfoResource::GetLangAndCodepages(std::vector<LANGANDCODEPAGE>& vecLangAndCodepage)
{
   ATLASSERT(IsAvail());

   UINT uiSize = 0;
   LPVOID pData = QueryValue(_T("\\VarFileInfo\\Translation"), &uiSize);

   ATLASSERT((uiSize % sizeof(LANGANDCODEPAGE)) == 0);

   LANGANDCODEPAGE* pLangAndCodepage = reinterpret_cast<LANGANDCODEPAGE*>(pData);

   for (unsigned int i=0; i<uiSize / sizeof(LANGANDCODEPAGE); i++)
      vecLangAndCodepage.push_back(pLangAndCodepage[i]);
}

CString VersionInfoResource::GetStringValue(const LANGANDCODEPAGE& langAndCodepage, LPCTSTR pszValueName)
{
   TCHAR szBuffer[256];
   _sntprintf_s(szBuffer, 256, 256,
      _T("\\StringFileInfo\\%04x%04x\\%s"),
      langAndCodepage.wLanguage, langAndCodepage.wCodePage,
      pszValueName);

   UINT uiSize = 0;
   LPCWSTR pszText = reinterpret_cast<LPCWSTR>(QueryValue(szBuffer, &uiSize));

//   ATLASSERT((uiSize & 1) == 0);
//   ATLASSERT(pszText[uiSize/sizeof(WCHAR)] == 0);
   ATLASSERT(uiSize == 0 || pszText[uiSize-1] == 0);

   USES_CONVERSION;

   return (pszText == NULL || *pszText == 0) ? CString(_T("???")) : CString(W2CT(pszText));
}
