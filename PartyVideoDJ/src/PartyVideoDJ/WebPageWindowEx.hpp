//
// PartyVideoDJ - YouTube crossfading app
// Copyright (C) 2008-2017 Michael Fink
//
/// \file WebPageWindowEx.hpp Extended web page window
//
#pragma once

#pragma once

#include <vector>
#include "WebPageWindow.hpp"

template <typename T, typename TIDocHostUIHandlerDispatchImpl>
class WebPageWindowEx : public WebPageWindow<T, TIDocHostUIHandlerDispatchImpl>
{
public:
   DECLARE_WND_SUPERCLASS(_T("WTL_WebPageView"), CAxWindow::GetWndClassName())

   BOOL PreTranslateMessage(MSG* pMsg)
   {
      if ((pMsg->message >= WM_KEYFIRST && pMsg->message <= WM_KEYLAST) &&
         (pMsg->message >= WM_MOUSEFIRST && pMsg->message <= WM_MOUSELAST))
         return TRUE;

      // give HTML page a chance to translate this message
      return (BOOL)SendMessage(WM_FORWARDMSG, 0, (LPARAM)pMsg);
   }

   void ExecuteJavaScript(LPCTSTR pszCode)
   {
      CComPtr<IHTMLDocument2> spIHTMLDocument2;
      HRESULT hr = GetHtmlDocument(spIHTMLDocument2);
      if (FAILED(hr))
         return;

      CComPtr<IHTMLWindow2> spIHTMLWindow2;
      hr = spIHTMLDocument2->get_parentWindow(&spIHTMLWindow2);
      if (FAILED(hr))
         return;

      CComBSTR bstrCode(pszCode), bstrLanguage(_T("javascript"));

      CComVariant varRet;
      hr = spIHTMLWindow2->execScript(bstrCode, bstrLanguage, &varRet);

      if (FAILED(hr))
         ATLTRACE(_T("execScript(\"%s\") returned hr=%08x\n"), pszCode, hr);
   }

   void CallFunction(LPCTSTR pszFunctionName, const std::vector<CComVariant>& vecParams, CComVariant& varResult)
   {
      CComPtr<IHTMLDocument2> spIHTMLDocument2;
      HRESULT hr = GetHtmlDocument(spIHTMLDocument2);
      if (FAILED(hr))
         return;

      CComPtr<IDispatch> spScript;
      spIHTMLDocument2->get_Script(&spScript);

      USES_CONVERSION;
      LPOLESTR pszoFunctionName = T2OLE(const_cast<LPTSTR>(pszFunctionName));
      DISPID dispidMethodToInvoke = 0;
      hr = spScript->GetIDsOfNames(IID_NULL, &pszoFunctionName, 1, LOCALE_SYSTEM_DEFAULT, &dispidMethodToInvoke);
      if (FAILED(hr))
         return;

      // invoke the script method
      DISPPARAMS dispParams = { NULL, NULL, 0, 0 };

      if (!vecParams.empty())
      {
         dispParams.cArgs = vecParams.size();

         // we're assigning the address of our vector here, since we already have all VARIANT's in order
         static_assert(sizeof(VARIANT) == sizeof(CComVariant), "VARIANT must have same size as CComVariant");
         dispParams.rgvarg = const_cast<CComVariant*>(&vecParams[0]);
      }

      hr = spScript->Invoke(dispidMethodToInvoke, IID_NULL,
         LOCALE_USER_DEFAULT, DISPATCH_METHOD, &dispParams,
         &varResult, NULL, NULL);

      if (hr != S_OK)
         ATLTRACE(_T("function call: %s() returned hr=%08x\n"), pszFunctionName, hr);
   }

   HRESULT GetHtmlDocument(CComPtr<IHTMLDocument2>& spDoc)
   {
      CComPtr<IWebBrowser2> spWebBrowser2;
      HRESULT hr = GetWebBrowser2(spWebBrowser2);

      CComPtr<IDispatch> spIDispatch;
      hr = spWebBrowser2->get_Document(&spIDispatch);
      if (FAILED(hr))
         return hr;

      return spIDispatch.QueryInterface<IHTMLDocument2>(&spDoc);
   }

   HRESULT GetElementById(const CString& cszId, CComPtr<IHTMLElement>& spElement)
   {
      CComPtr<IHTMLDocument2> spIHTMLDocument2;
      HRESULT hr = GetHtmlDocument(spIHTMLDocument2);
      if (FAILED(hr))
         return hr;

      CComPtr<IHTMLElementCollection> spIHTMLElementCollection;
      hr = spIHTMLDocument2->get_all(&spIHTMLElementCollection);
      if (FAILED(hr))
         return hr;

      CComBSTR bstrName(cszId);
      CComVariant varName(bstrName);
      CComVariant varIndex(static_cast<int>(0));

      CComPtr<IDispatch> spIDispatchIHTMLElement;
      hr = spIHTMLElementCollection->item(varName, varIndex, &spIDispatchIHTMLElement);
      if (FAILED(hr))
         return hr;

      if (spIDispatchIHTMLElement == NULL)
         return E_FAIL;

      return spIDispatchIHTMLElement.QueryInterface<IHTMLElement>(&spElement);
   }

private:
   BEGIN_MSG_MAP(WebPageWindowEx)
      COMMAND_ID_HANDLER(ID_EDIT_CUT, OnEditCut)
      COMMAND_ID_HANDLER(ID_EDIT_COPY, OnEditCopy)
      COMMAND_ID_HANDLER(ID_EDIT_PASTE, OnEditPaste)
      MESSAGE_RANGE_HANDLER(WM_MOUSEFIRST, WM_MOUSELAST, OnIgnoreMessage)
   END_MSG_MAP()

   // Handler prototypes (uncomment arguments if needed):
   // LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
   // LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
   // LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

   LRESULT OnEditCut(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
   {
      ExecWB(OLECMDID_CUT, OLECMDEXECOPT_DONTPROMPTUSER, NULL, NULL);
      return 0;
   }

   LRESULT OnEditCopy(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
   {
      ExecWB(OLECMDID_COPY, OLECMDEXECOPT_DONTPROMPTUSER, NULL, NULL);
      return 0;
   }

   LRESULT OnEditPaste(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
   {
      ExecWB(OLECMDID_PASTE, OLECMDEXECOPT_DONTPROMPTUSER, NULL, NULL);
      return 0;
   }

   LRESULT OnIgnoreMessage(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
   {
      bHandled = TRUE;
      return 0;
   }

private:
   HRESULT ExecWB(OLECMDID nCmd, OLECMDEXECOPT nCmdOptions, VARIANT* pvInput = NULL, VARIANT* pvOutput = NULL)
   {
      CComPtr<IWebBrowser2> spWebBrowser2;
      HRESULT hr = GetWebBrowser2(spWebBrowser2);
      if (FAILED(hr))
         return hr;

      return spWebBrowser2->ExecWB(nCmd, nCmdOptions, pvInput, pvOutput);
   }
};
