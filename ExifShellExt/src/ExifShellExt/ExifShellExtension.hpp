//
// ExifShellExt - Exif data shell extension
// Copyright (C) 2008 Michael Fink
//
/// \file ExifShellExtension.hpp EXIF shell extension implementation
//
#pragma once

// includes
#include <vector>

/// Shell extension implementation via COM
class ATL_NO_VTABLE ExifShellExtension:
    public CComObjectRootEx<CComSingleThreadModel>,
    public CComCoClass<ExifShellExtension, &CLSID_ExifShellExtension>,
    public IShellExtInit,
    public IContextMenu
{
public:
   ExifShellExtension();
   virtual ~ExifShellExtension();

   DECLARE_REGISTRY_RESOURCEID(IDR_SHELLEXTENSION)

   BEGIN_COM_MAP(ExifShellExtension)
      COM_INTERFACE_ENTRY(IShellExtInit)
      COM_INTERFACE_ENTRY_IID(IID_IContextMenu, IContextMenu)
   END_COM_MAP()

public:
   // IShellExtInit
   STDMETHODIMP Initialize(LPCITEMIDLIST, LPDATAOBJECT, HKEY);

   // IContextMenu
   STDMETHODIMP GetCommandString(UINT, UINT, UINT*, LPSTR, UINT);
   STDMETHODIMP InvokeCommand(LPCMINVOKECOMMANDINFO);
   STDMETHODIMP QueryContextMenu(HMENU, UINT, UINT, UINT, UINT);

private:
   void CopyExifToClipboard();
   void PasteExifFromClipboard();

private:
   /// filename
   CString m_cszFilename;

   /// "exif -> copy" menu id
   UINT m_uidMenuCopyExif;

   /// "exif -> paste" menu id
   UINT m_uidMenuPasteExif;

   /// exif menu
   CMenu m_menuExif;

   /// bitmap for exif menu
   CBitmap m_bmpExifIcon;

   /// exif clipboard format
   UINT m_uiExifClipboardFormat;
};
