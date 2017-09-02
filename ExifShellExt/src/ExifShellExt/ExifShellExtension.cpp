//
// ExifShellExt - Exif data shell extension
// Copyright (C) 2008 Michael Fink
//
/// \file ExifShellExtension.cpp EXIF shell extension implementation
//

// includes
#include "stdafx.h"
#include "resource.h"
#include "ExifShellExt_h.h"
#include "ExifShellExtension.hpp"
#include "Clipboard.hpp"
#include <ulib/stream/FileStream.hpp>
#include "ExifReader.hpp"
#include "ExifRewriter.hpp"

ExifShellExtension::ExifShellExtension()
:m_uidMenuCopyExif(0xFFFFFFFF),
 m_uidMenuPasteExif(0xFFFFFFFF)
{
   ATLTRACE(_T("ExifShellExtension() called\n"));

   Win32::Clipboard cb;
   m_uiExifClipboardFormat = cb.RegisterFormat(_T("CF_JPEG_EXIFDATA"));
}

ExifShellExtension::~ExifShellExtension()
{
   ATLTRACE(_T("~ExifShellExtension() called\n"));
}

STDMETHODIMP ExifShellExtension::Initialize(
   LPCITEMIDLIST /*pidlFolder*/, LPDATAOBJECT pDataObj, HKEY /*hProgID*/)
{
   FORMATETC fmt = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
   STGMEDIUM stg = { TYMED_HGLOBAL };

   // Look for CF_HDROP data in the data object.
   if (FAILED(pDataObj->GetData(&fmt, &stg)))
   {
      // Nope! Return an "invalid argument" error back to Explorer.
      return E_INVALIDARG;
   }

   // Get a pointer to the actual data.
   HDROP hDrop = (HDROP) GlobalLock(stg.hGlobal);

   // Make sure it worked.
   if (NULL == hDrop)
      return E_INVALIDARG;

   // Sanity check - make sure there is at least one filename.
   UINT uNumFiles = DragQueryFile(hDrop, 0xFFFFFFFF, NULL, 0);
   HRESULT hr = S_OK;

   if (1 != uNumFiles)
   {
      GlobalUnlock(stg.hGlobal);
      ReleaseStgMedium(&stg);
      return E_INVALIDARG;
   }

   // Get the name of the file and store it
   TCHAR szFile[MAX_PATH];
   UINT u = 0;
   if (0 == DragQueryFile(hDrop, u, szFile, MAX_PATH))
      hr = E_INVALIDARG;
   else
      m_cszFilename = szFile;

   GlobalUnlock(stg.hGlobal);
   ReleaseStgMedium(&stg);

   return hr;
}

STDMETHODIMP ExifShellExtension::QueryContextMenu(
   HMENU hMenu, UINT uMenuIndex, UINT uidFirstCmd,
   UINT /*uidLastCmd*/, UINT uFlags)
{
   ATLTRACE(_T("QueryContextMenu(hMenu=%08x, uMenuIndex=%u, uidFirstCmd=%08x, uidLastCmd, uFlags=%08x) called\n"),
      hMenu, uMenuIndex, uidFirstCmd, uFlags);

   // If the flags include CMF_DEFAULTONLY then we shouldn't do anything.
   if (uFlags & CMF_DEFAULTONLY)
      return MAKE_HRESULT ( SEVERITY_SUCCESS, FACILITY_NULL, 0 );

   UINT uCmdIndex = 0;

   if (!m_cszFilename.IsEmpty())
   {
      if (m_menuExif.IsNull())
      {
         ATLVERIFY(TRUE == m_menuExif.CreatePopupMenu());

         // menu entry: copy
         m_uidMenuCopyExif = uCmdIndex++;
         m_menuExif.AppendMenu(MF_BYCOMMAND, uidFirstCmd + m_uidMenuCopyExif, _T("Copy"));

         // check if there's exif data on the clipboard
         Win32::Clipboard cb;
         bool bFormatAvail = cb.IsFormatAvail(m_uiExifClipboardFormat);

         // menu entry: paste
         m_uidMenuPasteExif = uCmdIndex++;
         m_menuExif.AppendMenu(MF_BYCOMMAND | (bFormatAvail ? 0 : MF_GRAYED),
            uidFirstCmd + m_uidMenuPasteExif, _T("Paste"));
      }

      ::InsertMenu(hMenu, uMenuIndex, MF_BYPOSITION | MF_POPUP, (UINT_PTR)(HMENU)m_menuExif, _T("Exif"));
   }

   return MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_NULL, uCmdIndex);
}

STDMETHODIMP ExifShellExtension::GetCommandString(
   UINT idCmd, UINT uFlags, UINT* /*pwReserved*/, LPSTR pszName, UINT cchMax)
{
   // If Explorer is asking for a help string, copy our string into the
   // supplied buffer.
   if (uFlags & GCS_HELPTEXT)
   {
      CString cszText;
      if (idCmd == m_uidMenuCopyExif)
         cszText = _T("Copies Exif data to clipboard");
      else if (idCmd == m_uidMenuPasteExif)
         cszText = _T("Pastes Exif data from clipboard");
      else
         return E_INVALIDARG;

      USES_CONVERSION;
      if (uFlags & GCS_UNICODE)
      {
         // We need to cast pszName to a Unicode string, and then use the
         // Unicode string copy API.
         lstrcpynW(reinterpret_cast<LPWSTR>(pszName), T2CW(cszText), cchMax);
      }
      else
      {
         // Use the ANSI string copy API to return the help string.
         lstrcpynA(pszName, T2CA(cszText), cchMax);
      }

      return S_OK;
   }

   return E_INVALIDARG;
}

STDMETHODIMP ExifShellExtension::InvokeCommand(LPCMINVOKECOMMANDINFO pCmdInfo)
{
   ATLTRACE(_T("InvokeCommand with lpVerb = %u\n"), LOWORD(pCmdInfo->lpVerb));

   // If lpVerb really points to a string, ignore this function call and bail out.
   if (0 != HIWORD(pCmdInfo->lpVerb))
      return E_INVALIDARG;

   // Check command index
   UINT uidCmd = LOWORD(pCmdInfo->lpVerb);

   if (uidCmd == m_uidMenuCopyExif)
   {
      try
      {
         CopyExifToClipboard();
      }
      catch(...)
      {
      }
      return S_OK;
   }
   else
   if (uidCmd == m_uidMenuPasteExif)
   {
      try
      {
         PasteExifFromClipboard();
      }
      catch(...)
      {
      }
      return S_OK;
   }

   return E_INVALIDARG;
}

void ExifShellExtension::CopyExifToClipboard()
{
      Stream::FileStream streamIn(m_cszFilename, Stream::FileStream::modeOpen, Stream::FileStream::accessRead,
         Stream::FileStream::shareRead);

      ExifReader reader(streamIn);
      reader.Start();

      const std::vector<BYTE>& vecData = reader.GetData();

      Win32::Clipboard cb(GetActiveWindow());
      cb.Clear();
      cb.SetData(m_uiExifClipboardFormat, &vecData[0], static_cast<UINT>(vecData.size()));

      // TODO parse Exif and also put CF_TEXT infos on clipboard
}

void ExifShellExtension::PasteExifFromClipboard()
{
   Win32::Clipboard cb(GetActiveWindow());

   if (cb.IsFormatAvail(m_uiExifClipboardFormat))
   {
      std::vector<BYTE> vecData;
      cb.GetData(m_uiExifClipboardFormat, vecData);

      CString cszOutputFilename = m_cszFilename + _T(".exifshellext-rename");

      // rewrite jpeg file using new exif data
      {
         Stream::FileStream streamIn(m_cszFilename, Stream::FileStream::modeOpen, Stream::FileStream::accessRead,
            Stream::FileStream::shareRead);

         Stream::FileStream streamOut(cszOutputFilename, Stream::FileStream::modeCreateNew, Stream::FileStream::accessWrite,
            Stream::FileStream::shareRead);

         ExifRewriter rewriter(streamIn, streamOut, vecData);
         rewriter.Start();
      }

      // TODO now set old file date/time

      // rename
      ::DeleteFile(m_cszFilename);
      ::MoveFile(cszOutputFilename, m_cszFilename);
   }
}
