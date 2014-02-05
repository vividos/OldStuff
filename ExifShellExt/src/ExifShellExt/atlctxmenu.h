#ifndef _ATLCTXMENU_H__
#define _ATLCTXMENU_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __cplusplus
	#error ATL requires C++ compilation (use a .cpp suffix)
#endif

#ifndef __ATLBASE_H__
	#error atctxmenu.h requires atlbase.h to be included first
#endif

#include <shlobj.h>
#include <comdef.h>

#define BEGIN_MENU_ITEM_MAP()	\
public: \
	const static _ATL_MENU_ITEM_ENTRY* WINAPI GetContextMenuEntries() { \
		static const _ATL_MENU_ITEM_ENTRY _entries[] = {

#define MENU_ITEM_ENTRY(idCmd, idDescription)	\
			{ idCmd, idDescription },

#define END_MENU_ITEM_MAP()	\
			{ 0, 0 } \
		}; \
		return _entries; \
	}

#define BEGIN_MENU_COMMAND_MAP()	\
public: \
	HRESULT _InvokeCommand(UINT nidCmd, LPCMINVOKECOMMANDINFO lpici) \
	{ \
		HRESULT hResult = S_OK;

#define MENU_COMMAND_ENTRY(idCmd, func)	\
		if (nidCmd == idCmd) { \
			hResult = func(lpici); \
			if (FAILED(hResult)) return hResult; \
		}

#define END_MENU_COMMAND_MAP()	\
		return S_OK; \
	}

//////////////////////////////////////////////////////////////////////////////
// IContextMenuImpl

template <class T>
class ATL_NO_VTABLE IContextMenuImpl : public IContextMenu
{
public:
	STDMETHOD(GetCommandString)(UINT idCmd, UINT uFlags, UINT* /*pwReserved*/, LPSTR pszName, UINT cchMax)
	{
		const _ATL_MENU_ITEM_ENTRY* pEntries = T::GetContextMenuEntries();
		
		UINT nItems = 0;
		while (pEntries->idCmd != 0 && nItems < idCmd) {
			nItems++;
			pEntries++;
		}

		if (pEntries->idCmd == 0)
			return E_INVALIDARG;
		
		if (uFlags & GCS_HELPTEXT) {

			USES_CONVERSION;
			TCHAR szDescription[MAX_PATH];
			::LoadString(_Module.GetResourceInstance(),
						pEntries->idDescription,
						szDescription, MAX_PATH);
			ATLTRACE("menu item: %d\n", szDescription);

			if (uFlags & GCS_UNICODE) {

				// We need to cast pszName to a Unicode string, and then use the
				// Unicode string copy API.
				lstrcpynW((LPWSTR) pszName, T2CW(szDescription), cchMax);
				return S_OK;

			} else {

				// Use the ANSI string copy API to return the help string.
				lstrcpynA(pszName, T2CA(szDescription), cchMax);
				return S_OK;
			}
		}

		return E_INVALIDARG;
	}
	STDMETHOD(InvokeCommand)(LPCMINVOKECOMMANDINFO lpici)
	{
		if (HIWORD(lpici->lpVerb) != 0) return E_INVALIDARG;

		UINT idCmd = LOWORD(lpici->lpVerb);
		UINT nItems = 0;

		const _ATL_MENU_ITEM_ENTRY* pEntries = T::GetContextMenuEntries();

		while (pEntries->idCmd != 0 && nItems < idCmd) {
			nItems++;
			pEntries++;
		}

		if (pEntries->idCmd == 0)
			return E_INVALIDARG;

		T* pThis = (T*) this;
		return pThis->_InvokeCommand(pEntries->idCmd, lpici);
	}
	STDMETHOD(QueryContextMenu)(HMENU hmenu, UINT indexMenu, UINT idCmdFirst, UINT idCmdLast, UINT uFlags)
	{
		if (uFlags & CMF_DEFAULTONLY)
			return MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_NULL, 0);

		UINT nItems = 0;

		const _ATL_MENU_ITEM_ENTRY* pEntries = T::GetContextMenuEntries();
		while (pEntries->idCmd != 0) {
			TCHAR szCmd[MAX_PATH];
			::LoadString(_Module.GetResourceInstance(),
								pEntries->idCmd,
								szCmd, MAX_PATH);

			::InsertMenu(hmenu, indexMenu, MF_BYPOSITION,
								idCmdFirst + nItems,
								szCmd);

			pEntries++;
			nItems++;
		}

		return MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_NULL, nItems);
	}

public:
	struct _ATL_MENU_ITEM_ENTRY {
		UINT idCmd;
		UINT idDescription;
	};

};

#endif // _ATLCTXMENU_H__