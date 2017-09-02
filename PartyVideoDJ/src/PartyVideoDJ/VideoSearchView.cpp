//
// PartyVideoDJ - YouTube crossfading app
// Copyright (C) 2008-2017 Michael Fink
//
/// \file VideoSearchView.cpp View to search for videos
//
#include "stdafx.h"
#include "resource.h"
#include "VideoSearchView.hpp"

void VideoSearchView::SearchVideo(const CString& cszText)
{
   ATLVERIFY(true == Navigate(_T("http://www.youtube.com/results?search_query=") + cszText));

   SetFocus(); // set focus so that user can start scrolling away
}

BOOL VideoSearchView::PreTranslateMessage(MSG* pMsg)
{
   if ((pMsg->message < WM_KEYFIRST || pMsg->message > WM_KEYLAST) &&
      (pMsg->message < WM_MOUSEFIRST || pMsg->message > WM_MOUSELAST))
      return FALSE;

   // give HTML page a chance to translate this message
   return (BOOL)SendMessage(WM_FORWARDMSG, 0, (LPARAM)pMsg);
}

LRESULT VideoSearchView::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
   PostMessage(WM_INIT_VIEW);

   bHandled = FALSE;
   return 0;
}

LRESULT VideoSearchView::OnInitView(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   Init();

   return 0;
}

bool GetAnchorTitleByElement(CComPtr<IHTMLElement>& spElement, CString& cszTitle)
{
   CComBSTR bstrTagName;
   HRESULT hr = spElement->get_tagName(&bstrTagName);

   CString cszTagName(bstrTagName);
   cszTagName.MakeLower();

   if (cszTagName == _T("a"))
   {
      // get title attr
      CComBSTR bstrTitleAttr(_T("title"));

      CComVariant varTitle;
      hr = spElement->getAttribute(bstrTitleAttr, 0, &varTitle);
      if (SUCCEEDED(hr))
      {
         cszTitle = CComBSTR(varTitle.bstrVal);
         if (!cszTitle.IsEmpty())
            return true;
      }

      // get inner element and strip all html
      CComBSTR bstrText;
      if (SUCCEEDED(spElement->get_innerText(&bstrText)))
      {
         cszTitle = bstrText;
         if (!cszTitle.IsEmpty())
            return true;
      }
   }
   else
   {
      // check all child elements
      CComPtr<IDispatch> spIDispColl;
      hr = spElement->get_children(&spIDispColl);

      CComPtr<IHTMLElementCollection> spElementCollection;
      hr = spIDispColl.QueryInterface(&spElementCollection);

      if (SUCCEEDED(hr))
      {
         long lIndex = 0, lMax = 0;
         spElementCollection->get_length(&lMax);
         for (; lIndex < lMax; lIndex++)
         {
            CComVariant varName, varIndex;
            varIndex.ChangeType(VT_I4);
            varIndex.lVal = lMax;

            CComPtr<IDispatch> spIDispChildElement;
            hr = spElementCollection->item(varName, varIndex, &spIDispChildElement);

            CComPtr<IHTMLElement> spChildElement;
            hr = spIDispChildElement.QueryInterface(&spChildElement);
            if (SUCCEEDED(hr))
            {
               bool bFound = GetAnchorTitleByElement(spChildElement, cszTitle);
               if (bFound)
                  return true;
            }
         }
      }
   }

   return false;
}

void VideoSearchView::OnBeforeNavigate2(const CString& cszURL_, const CString& cszTargetFrameName, bool& bCancel)
{
   CString cszURL(cszURL_);

   ATLTRACE(_T("OnBeforeNavigate2(url = \"%s\") called\n"), cszURL.GetString());

   if (!cszTargetFrameName.IsEmpty())
   {
      // don't open new frame
      bCancel = true;
      return;
   }

   if (cszURL.Find(_T("doubleclick.net")) != -1)
   {
      // don't load ad sites
      bCancel = true;
      return;
   }

   if (cszURL.Find(_T("search_query")) != -1)
   {
      // this one's ok
      return;
   }

   if (cszURL.Find(_T("watch?v=")) != -1 ||
      (cszURL.Find(_T("view_play_list")) != -1 &&
      (cszURL.Find(_T("&v=")) != -1))
      )
   {
      bCancel = true;

      CPoint pt;
      GetCursorPos(&pt);

      ScreenToClient(&pt);

      CComPtr<IHTMLDocument2> spDoc;
      HRESULT hr = GetHtmlDocument(spDoc);

      CComPtr<IHTMLElement> spElement;
      hr = spDoc->elementFromPoint(pt.x, pt.y, &spElement);

      if (SUCCEEDED(hr) && spElement != NULL)
      {
         // show context menu
         CString cszTitle;
         bool bRet = GetAnchorTitleByElement(spElement, cszTitle);
         if (!bRet)
         {
            // try parent item
            CComPtr<IHTMLElement> spParentElement;
            hr = spElement->get_parentElement(&spParentElement);
            if (SUCCEEDED(hr))
               bRet = GetAnchorTitleByElement(spParentElement, cszTitle);
         }

         cszTitle.TrimLeft();
         cszTitle.TrimRight();

         if (cszTitle.IsEmpty())
            return;

         ReplaceHtmlEntities(cszTitle);

         if (bRet)
         {
            int iRet = AtlMessageBox(m_hWnd, (LPCTSTR)cszTitle, _T("Add this link to Titles?"), MB_YESNO | MB_ICONQUESTION);
            if (iRet == IDYES)
            {
               if (m_fnAddTitle != NULL)
               {
                  // get youtube id
                  int iPos = cszURL.Find(_T("v="));
                  if (iPos != -1)
                     cszURL = cszURL.Mid(iPos + 2);

                  iPos = cszURL.Find(_T(";"));
                  if (iPos != -1)
                     cszURL = cszURL.Left(iPos);

                  iPos = cszURL.Find(_T("&"));
                  if (iPos != -1)
                     cszURL = cszURL.Left(iPos);

                  m_fnAddTitle(cszTitle, cszURL);
               }
            }

            // re-focus on webbrowser
            SetFocus();
         }
         else
            AtlMessageBox(m_hWnd, (LPCTSTR)cszURL, _T("Invalid link"), MB_OK | MB_ICONEXCLAMATION);
      }
   }
}

void VideoSearchView::ReplaceHtmlEntities(CString& cszText)
{
   cszText.Replace(_T("&szlig;"), _T("ß"));
   cszText.Replace(_T("&amp;"), _T("&"));
   cszText.Replace(_T("&#39;"), _T("'"));
}
