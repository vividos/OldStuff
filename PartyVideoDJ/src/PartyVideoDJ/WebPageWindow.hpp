//
// PartyVideoDJ - YouTube crossfading app
// Copyright (C) 2008-2017 Michael Fink
//
/// \file WebPageWindow.hpp Window to display web page
//
#pragma once

#include <exdispid.h>
#include <ulib/win32/DocHostUI.hpp>

/// dispatch event id
#define ID_BROWSER 1

/// web page window traits
typedef CWinTraits<WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, 0> WebPageWinTraits;

template <typename T, typename TIDocHostUIHandlerDispatchImpl = IDocHostUIHandlerDispatchImpl>
class WebPageWindow :
   public CWindowImpl<WebPageWindow<T, TIDocHostUIHandlerDispatchImpl>, CAxWindow, WebPageWinTraits>,
   public IDispEventImpl<ID_BROWSER, WebPageWindow<T, TIDocHostUIHandlerDispatchImpl> >,
   public TIDocHostUIHandlerDispatchImpl
{
public:
   typedef CWindowImpl<WebPageWindow<T, TIDocHostUIHandlerDispatchImpl>, CAxWindow, WebPageWinTraits> BaseClass;

   DECLARE_WND_SUPERCLASS(NULL, CAxWindow::GetWndClassName())

   void Init()
   {
      HRESULT hr = SetExternalUIHandler(this);
      ATLVERIFY(SUCCEEDED(hr));

      AtlAdviseSinkMap(this, true);

      // Don't allow user to drop shortcut onto this window and have the browser
      // navigate to show that web site
      {
         CComPtr<IWebBrowser2> spWebBrowser2;
         HRESULT hr2 = GetWebBrowser2(spWebBrowser2);
         ATLVERIFY(SUCCEEDED(hr2));
         ATLASSERT(spWebBrowser2 != NULL);

         spWebBrowser2->put_RegisterAsDropTarget(VARIANT_FALSE);
      }
   }

   HRESULT GetWebBrowser2(CComPtr<IWebBrowser2>& spWebBrowser2)
   {
      return QueryControl<IWebBrowser2>(&spWebBrowser2);
   }

   bool Navigate(const CString& cszURL)
   {
      CComPtr<IWebBrowser2> spWebBrowser2;
      HRESULT hr = GetWebBrowser2(spWebBrowser2);
      if (SUCCEEDED(hr))
      {
         CComBSTR bstrURL(cszURL);
         CComVariant varURL(bstrURL);
         hr = spWebBrowser2->Navigate2(&varURL, NULL, NULL, NULL, NULL);
         return SUCCEEDED(hr);
      }
      else
         return false;
   }

   void OnBeforeNavigate2(const CString& /*cszURL*/, const CString& /*cszTargetFrameName*/, bool& bCancel)
   {
      bCancel = false;
   }

   void OnNewWindow3(const CString& /*cszURL*/, bool& bCancel)
   {
      bCancel = false;
   }

public:
   // for AtlAdviseSinkMap to work
   HWND GetDlgItem(int nID)
   {
      if (nID == ID_BROWSER)
         return m_hWnd;
      else
         return BaseClass::GetDlgItem(nID);
   }

private:
   BEGIN_MSG_MAP(WebPageWindow)
   END_MSG_MAP()

   BEGIN_SINK_MAP(WebPageWindow)
      SINK_ENTRY(ID_BROWSER, DISPID_BEFORENAVIGATE2, &OnBeforeNavigate2_)
      SINK_ENTRY(ID_BROWSER, DISPID_TITLECHANGE, &OnBrowserTitleChange)
      SINK_ENTRY(ID_BROWSER, DISPID_NAVIGATECOMPLETE2, &OnNavigateComplete2)
      SINK_ENTRY(ID_BROWSER, DISPID_DOCUMENTCOMPLETE, &OnDocumentComplete)
      SINK_ENTRY(ID_BROWSER, DISPID_COMMANDSTATECHANGE, &OnCommandStateChange)
      //SINK_ENTRY(ID_BROWSER, DISPID_NEWWINDOW, &OnNewWindow)
      //SINK_ENTRY(ID_BROWSER, DISPID_NEWWINDOW2, &OnNewWindow2)
      SINK_ENTRY(ID_BROWSER, DISPID_NEWWINDOW3, &OnNewWindow3_)
      SINK_ENTRY(ID_BROWSER, DISPID_NAVIGATEERROR, &OnNavigateError)
   END_SINK_MAP()

private:
   void __stdcall OnBeforeNavigate2_(IDispatch* /*pDisp*/, VARIANT * pvtURL,
      VARIANT * /*pvtFlags*/, VARIANT * pvtTargetFrameName,
      VARIANT * /*pvtPostData*/, VARIANT * /*pvtHeaders*/,
      VARIANT_BOOL* pvbCancel)
   {
      CString cszTargetFrameName;
      if (pvtTargetFrameName != NULL)
         cszTargetFrameName = pvtTargetFrameName->bstrVal;

      bool bCancel = false;

      T* pT = static_cast<T*>(this);
      pT->OnBeforeNavigate2(
         CString(pvtURL->bstrVal),
         cszTargetFrameName,
         bCancel
      );

      *pvbCancel = bCancel ? VARIANT_TRUE : VARIANT_FALSE;
   }

   void __stdcall OnBrowserTitleChange(BSTR /*bstrTitleText*/) {}
   void __stdcall OnNavigateComplete2(IDispatch* /*pDisp*/, VARIANT* /*pvtURL*/) {}
   void __stdcall OnDocumentComplete(IDispatch* /*pDisp*/, VARIANT* /*pvtURL*/) {}
   void __stdcall OnCommandStateChange(long /*lCommand*/, VARIANT_BOOL /*bEnable*/) {}
   //   void __stdcall OnNewWindow(IDispatch** /*ppDisp*/, VARIANT_BOOL* pvbCancel){}
   //   void __stdcall OnNewWindow2(IDispatch** /*ppDisp*/, VARIANT_BOOL* pvbCancel){}
   void __stdcall OnNewWindow3_(IDispatch** /*ppDisp*/, VARIANT_BOOL* pvbCancel,
      DWORD /*dwFlags*/, BSTR /*bstrUrlContext*/, BSTR bstrUrl)
   {
      bool bCancel = false;

      CString cszURL(bstrUrl);

      T* pT = static_cast<T*>(this);
      pT->OnNewWindow3(cszURL, bCancel);

      *pvbCancel = bCancel ? VARIANT_TRUE : VARIANT_FALSE;
   }

   void __stdcall OnNavigateError(IDispatch* /*pDisp*/,
      VARIANT* /*URL*/,
      VARIANT* /*TargetFrameName*/,
      VARIANT* /*StatusCode*/,
      VARIANT_BOOL* pvbCancel)
   {
      *pvbCancel = VARIANT_TRUE;
   }
};
