//
// PartyVideoDJ - YouTube crossfading app
// Copyright (C) 2008-2017 Michael Fink
//
/// \file Win7Taskbar.cpp Windows 7 task bar access
//
#include "stdafx.h"
#include "Win7Taskbar.hpp"
#include <ShObjIdl.h>

#if 0
#define OLD_NTDDI_VERSION NTDDI_VERSION
#undef NTDDI_VERSION
#define NTDDI_VERSION NTDDI_WIN7
#include "C:\Program Files\Microsoft SDKs\Windows\v7.0\Include\ShObjIdl.h"
#undef NTDDI_VERSION
#define NTDDI_VERSION OLD_NTDDI_VERSION

typedef interface ITaskbarList3 ITaskbarList3;

typedef /* [v1_enum] */
enum TBPFLAG
{
   TBPF_NOPROGRESS = 0,
   TBPF_INDETERMINATE = 0x1,
   TBPF_NORMAL = 0x2,
   TBPF_ERROR = 0x4,
   TBPF_PAUSED = 0x8
} TBPFLAG;

EXTERN_C const IID IID_ITaskbarList3;

MIDL_INTERFACE("ea1afb91-9e28-4b86-90e9-9e9f8a5eefaf")
ITaskbarList3 : public ITaskbarList2
{
public:
    virtual HRESULT STDMETHODCALLTYPE SetProgressValue(
       /* [in] */ __RPC__in HWND hwnd,
       /* [in] */ ULONGLONG ullCompleted,
       /* [in] */ ULONGLONG ullTotal) = 0;

   virtual HRESULT STDMETHODCALLTYPE SetProgressState(
      /* [in] */ __RPC__in HWND hwnd,
      /* [in] */ TBPFLAG tbpFlags) = 0;

  virtual HRESULT STDMETHODCALLTYPE RegisterTab(
     /* [in] */ __RPC__in HWND hwndTab,
     /* [in] */ __RPC__in HWND hwndMDI) = 0;

 virtual HRESULT STDMETHODCALLTYPE UnregisterTab(
    /* [in] */ __RPC__in HWND hwndTab) = 0;

virtual HRESULT STDMETHODCALLTYPE SetTabOrder(
   /* [in] */ __RPC__in HWND hwndTab,
   /* [in] */ __RPC__in HWND hwndInsertBefore) = 0;

virtual HRESULT STDMETHODCALLTYPE SetTabActive(
   /* [in] */ __RPC__in HWND hwndTab,
   /* [in] */ __RPC__in HWND hwndMDI,
   /* [in] */ DWORD dwReserved) = 0;
//#if 0
        virtual HRESULT STDMETHODCALLTYPE ThumbBarAddButtons(
           /* [in] */ __RPC__in HWND hwnd,
           /* [in] */ UINT cButtons,
           /* [size_is][in] */ __RPC__in_ecount_full(cButtons) LPTHUMBBUTTON pButton) = 0;

       virtual HRESULT STDMETHODCALLTYPE ThumbBarUpdateButtons(
          /* [in] */ __RPC__in HWND hwnd,
          /* [in] */ UINT cButtons,
          /* [size_is][in] */ __RPC__in_ecount_full(cButtons) LPTHUMBBUTTON pButton) = 0;

      virtual HRESULT STDMETHODCALLTYPE ThumbBarSetImageList(
         /* [in] */ __RPC__in HWND hwnd,
         /* [in] */ __RPC__in_opt HIMAGELIST himl) = 0;

     virtual HRESULT STDMETHODCALLTYPE SetOverlayIcon(
        /* [in] */ __RPC__in HWND hwnd,
        /* [in] */ __RPC__in HICON hIcon,
        /* [string][unique][in] */ __RPC__in_opt_string LPCWSTR pszDescription) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetThumbnailTooltip(
       /* [in] */ __RPC__in HWND hwnd,
       /* [string][unique][in] */ __RPC__in_opt_string LPCWSTR pszTip) = 0;

   virtual HRESULT STDMETHODCALLTYPE SetThumbnailClip(
      /* [in] */ __RPC__in HWND hwnd,
      /* [in] */ __RPC__in RECT *prcClip) = 0;

};
#endif


using Win7::TaskbarProgressBar;
using Win7::TaskbarImpl;
using Win7::Taskbar;

class TaskbarImpl
{
public:
   HWND m_hWnd;
   CComPtr<ITaskbarList3> m_spTaskBarlist;
};

void TaskbarProgressBar::SetState(TaskbarProgressBarState enState)
{
   ATLASSERT(
      enState == TBPF_NOPROGRESS ||
      enState == TBPF_INDETERMINATE ||
      enState == TBPF_NORMAL ||
      enState == TBPF_ERROR ||
      enState == TBPF_PAUSED);

   m_spImpl->m_spTaskBarlist->SetProgressState(m_spImpl->m_hWnd,
      static_cast<TBPFLAG>(enState));
}

void TaskbarProgressBar::SetPos(UINT uiPos, UINT uiMax)
{
   m_spImpl->m_spTaskBarlist->SetProgressValue(m_spImpl->m_hWnd, uiPos, uiMax);
}

Taskbar::Taskbar(HWND hWnd) throw()
   :m_spImpl(new TaskbarImpl)
{
   m_spImpl->m_hWnd = hWnd;
   HRESULT hr = m_spImpl->m_spTaskBarlist.CoCreateInstance(CLSID_TaskbarList, NULL, CLSCTX_ALL);
   ATLVERIFY(SUCCEEDED(hr));
}

bool Taskbar::IsAvailable() const throw()
{
   return m_spImpl->m_spTaskBarlist != NULL;
}
