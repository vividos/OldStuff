//
// ulib - a collection of useful classes
// Copyright (C) 2007,2008 Michael Fink
//
/// \file Clipboard.hpp clipboard class
//
#pragma once

// includes
#include <vector>
#include <boost/noncopyable.hpp>

namespace Win32
{

/// \brief clipboard access
/// \note when creating this object, the process has exclusive access to the
/// clipboard, so only create when needed
class Clipboard: public boost::noncopyable
{
public:
   /// ctor; opens clipboard
   Clipboard(HWND hWnd = NULL) throw()
   {
      ATLVERIFY(TRUE == OpenClipboard(hWnd));
   }

   /// dtor
   ~Clipboard() throw()
   {
      BOOL bRet = CloseClipboard();
      ATLVERIFY(TRUE == bRet);
   }

   /// empties the clipboard
   void Clear() const throw()
   {
      ATLVERIFY(TRUE == EmptyClipboard());
   }

   /// returns number of formats currently on the clipboard
   int CountFormats() const throw()
   {
      return CountClipboardFormats();
   }

   /// checks if a given format is available
   static bool IsFormatAvail(UINT uiFormat) throw()
   {
      return FALSE != IsClipboardFormatAvailable(uiFormat);
   }

   /// returns text format (CF_TEXT or CF_UNICODETEXT)
   CString GetText()
   {
#ifdef _UNICODE
      UINT uiFormat = CF_TEXT;
#else
      UINT uiFormat = CF_UNICODETEXT;
#endif
      HANDLE hglb = GetClipboardData(uiFormat);
      if (hglb != NULL)
      {
         LPVOID pData = GlobalLock(hglb);
         if (pData != NULL) 
         {
            CString cszText(reinterpret_cast<LPCTSTR>(pData));

            GlobalUnlock(hglb);

            return cszText;
         }
      }

      return _T("");
   }

   /// returns binary data for given clipboard format
   void GetData(UINT uiFormat, std::vector<BYTE>& vecData)
   {
      ATLASSERT(TRUE == IsClipboardFormatAvailable(uiFormat));

      HANDLE hglb = GetClipboardData(uiFormat);
      if (hglb != NULL)
      {
         LPVOID pData = GlobalLock(hglb);
         if (pData != NULL) 
         {
            SIZE_T uiSize = GlobalSize(hglb);

            vecData.resize(uiSize);
            memcpy(&vecData[0], pData, uiSize);

            GlobalUnlock(hglb);
         }
      }
   }

   /// sets clipboard text as format CF_TEXT or CF_UNICODETEXT (depending on build options)
   void SetText(const CString& cszText) throw()
   {
      USES_CONVERSION;
#ifdef _UNICODE
      LPCTSTR pszwData = T2CW(cszText);
      SetData(CF_UNICODETEXT, reinterpret_cast<const BYTE*>(pszwData),
         static_cast<UINT>(wcslen(pszwData) + 1*sizeof(*pszwData)));
#else
      LPCSTR pszaData = T2CA(cszText);
      SetData(CF_TEXT, pszaData, strlen(pszaData) + 1*sizeof(*pszaData));
#endif
   }

   /// \todo not implemented
   void SetHtml(const CString& cszHtmlFragment, const CString& cszSourceURL)
   {
      cszHtmlFragment;
      cszSourceURL;
      // format see here:
      // http://msdn.microsoft.com/en-us/library/ms649015(VS.85).aspx

      // now convert everything to UTF8
   }

   /// sets clipboard data; generic function
   void SetData(UINT uFormat, const BYTE* pbData, UINT uiSize) throw()
   {
      HGLOBAL hglbCopy = GlobalAlloc(GMEM_MOVEABLE, uiSize);

      LPVOID pData = GlobalLock(hglbCopy); 
      memcpy(pData, pbData, uiSize);
      GlobalUnlock(hglbCopy);

      // Place the handle on the clipboard.
      SetClipboardData(uFormat, hglbCopy);
   }

   /// registers a given format name
   UINT RegisterFormat(LPCTSTR pszFormatName) const throw()
   {
      return RegisterClipboardFormat(pszFormatName);
   }

   /// returns format name for given format id
   CString GetFormatName(UINT uiFormat) const throw()
   {
      CString cszName;
      ATLVERIFY(TRUE == GetClipboardFormatName(uiFormat, cszName.GetBufferSetLength(256), 256));
      cszName.ReleaseBuffer();
   }

   /// enumerates all clipboard formats currently available on the clipboard
   void EnumFormats(std::vector<UINT>& vecFormats) const throw()
   {
      UINT uiFormat = 0;
      while (0 != (uiFormat = EnumClipboardFormats(uiFormat)))
         vecFormats.push_back(uiFormat);
   }
};

} // namespace Win32
