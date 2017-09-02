#ifndef _ATLSHELLEX_H__
#define _ATLSHELLEX_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __cplusplus
	#error ATL requires C++ compilation (use a .cpp suffix)
#endif

#ifndef __ATLBASE_H__
	#error atctxmenu.h requires atlbase.h to be included first
#endif

#ifndef __ATLMISC_H__
	#error atctxmenu.h requires atlmisc.h to be included first
#endif

#include <shlobj.h>

#define DECLARE_REGISTRY_SHELL_EXTENSION(id, ext, name, clsid)	\
static HRESULT WINAPI UpdateRegistry(BOOL bRegister) { \
		DWORD dwSize = 40; \
		TCHAR szFile[40]; \
		if (!RegFileFromExtension(_T(".") _T(#ext), szFile, &dwSize)) \
			return E_INVALIDARG; \
		USES_CONVERSION; \
		_ATL_REGMAP_ENTRY regMap [] = { \
			{ OLESTR("FILE"), T2COLE(szFile) }, \
			{ OLESTR("NAME"), T2COLE(_T(#name)) }, \
			{ 0, 0 } \
		}; \
		HRESULT hResult = _Module.UpdateRegistryFromResource(id, bRegister, regMap); \
		if (FAILED(hResult)) return hResult; \
		return ApproveShellExtension(clsid, _T(#name), bRegister); \
	}

////////////////////////////////////////////////////////////////////////
// IShellExtInitImpl

class ATL_NO_VTABLE IShellExtInitImpl : public IShellExtInit
{
// Construction / destruction
public:
	IShellExtInitImpl() {}
	virtual ~IShellExtInitImpl() { m_files.RemoveAll(); }

// IShellExtInit
public:
	STDMETHOD(Initialize)(LPCITEMIDLIST /*pidlFolder*/, LPDATAOBJECT lpdobj, HKEY /*hkeyProgID*/)
	{
		FORMATETC fmt = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
		STGMEDIUM stg = { TYMED_HGLOBAL };

		HRESULT hResult = lpdobj->GetData(&fmt, &stg);
		if (FAILED(hResult)) return E_INVALIDARG;

		HDROP hDrop = (HDROP) ::GlobalLock(stg.hGlobal);
		if (hDrop == 0) return E_INVALIDARG;

		UINT nFiles = ::DragQueryFile(hDrop, 0xFFFFFFFF, NULL, 0);
		if (nFiles == 0) {
			::GlobalUnlock(stg.hGlobal);
			::ReleaseStgMedium(&stg);
			return E_INVALIDARG;
		}

		UINT n;
		for (n = 0; n < nFiles; n++) {
			CString aFile;
			LPTSTR szFile = aFile.GetBuffer(MAX_PATH);
			::DragQueryFile(hDrop, n, szFile, MAX_PATH);
			aFile.ReleaseBuffer();

			// TODO: Override IsValidFile() in your class
			if (IsValidFile(aFile)) m_files.Add(aFile);
		}

		// Release memory

		::GlobalUnlock(stg.hGlobal);
		::ReleaseStgMedium(&stg);

		return (m_files.GetSize() > 0 ? S_OK : E_INVALIDARG);
	}
	
// Overrides
protected:
	virtual bool IsValidFile(CString& aFile) const = 0;

// Registry Helpers
protected:
	static HRESULT ApproveShellExtension(const CLSID& clsid, LPCTSTR szName, BOOL bRegister)
	{
		if (WinNT4()) {

			USES_CONVERSION;

			LPCTSTR szApproved = _T("Software\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved");
			LPOLESTR ozCLSID;
			::StringFromCLSID(clsid, &ozCLSID);

			if (bRegister) {
				TCHAR szValue[MAX_PATH];
				lstrcpy(szValue, szName);
				lstrcat(szValue, _T(" Shell Extension"));
				CRegKey::SetValue(HKEY_LOCAL_MACHINE, szApproved, szValue, OLE2T(ozCLSID));
			} else {
				CRegKey hKey;
				hKey.Open(HKEY_LOCAL_MACHINE, szApproved);
				hKey.DeleteValue(OLE2T(ozCLSID));
				hKey.Close();
			}

			::CoTaskMemFree(ozCLSID);
		}

		return S_OK;
	}

	static bool RegFileFromExtension(LPCTSTR szExt, LPTSTR szFile, DWORD* pdwSize)
	{
		CRegKey hKey;
		LONG nResult = hKey.Open(HKEY_CLASSES_ROOT, szExt);
		if (nResult != ERROR_SUCCESS) return FALSE;

		nResult = hKey.QueryStringValue(NULL, szFile, pdwSize);
		if (nResult != ERROR_SUCCESS) return FALSE;
		hKey.Close();

		return TRUE;
	}

	static bool WinNT4(void)
	{
		return true;
	}

// Class members
protected:
	CSimpleArray<CString>	m_files;
};

#endif // _ATLSHELLEX_H__