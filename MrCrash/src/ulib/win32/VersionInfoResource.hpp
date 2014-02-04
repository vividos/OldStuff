//
// ulib - a collection of useful classes
// Copyright (C) 2004-2014 Michael Fink
//
/// \file VersionInfoResource.hpp version info resource class
//
#pragma once

// includes
#include <vector>

namespace Win32
{

/// struct wrapping the language and codepage identifier
typedef struct tagLANGANDCODEPAGE
{
   WORD wLanguage; ///< language code
   WORD wCodePage; ///< code page
} LANGANDCODEPAGE, *PLANGANDCODEPAGE;

/// \brief resource info that contain language-agnostic version infos
/// wraps win32 VS_FIXEDFILEINFO struct
class FixedFileInfo: public VS_FIXEDFILEINFO
{
public:
   /// returns formatted file version
   CString GetFileVersion() const;

   /// returns formatted product version
   CString GetProductVersion() const;

   /// returns VS_FIXEDFILEINFO::dwFileOS as text representation
   CString GetFileOS() const;

   /// returns VS_FIXEDFILEINFO::dwFileType and VS_FIXEDFILEINFO::dwFileSubtype as text representation
   CString GetFileType() const;
};

/// version info resource of file PE image (.dll, .exe)
class VersionInfoResource
{
public:
   /// ctor
   VersionInfoResource(LPCTSTR pszFilename);

   /// returns if version info is available
   bool IsAvail() const { return !m_vecVersionInfoData.empty(); }

   /// returns fixed file info
   FixedFileInfo* GetFixedFileInfo()
   {
      return reinterpret_cast<FixedFileInfo*>(QueryValue(_T("\\")));
   }

   /// returns all language / codepage pairs of language-dependent version info blocks
   void GetLangAndCodepages(std::vector<LANGANDCODEPAGE>& vecLangAndCodepage);

   /// returns string value of given language-dependent resource
   CString GetStringValue(const LANGANDCODEPAGE& langAndCodepage, LPCTSTR pszValueName);

private:
   /// queries data from subblock
   LPVOID QueryValue(LPCTSTR pszSubBlock, UINT* puiSize = NULL);

private:
   /// raw version info data
   std::vector<BYTE> m_vecVersionInfoData;
};

} // namespace Tools
