//
// PartyVideoDJ - YouTube crossfading app
// Copyright (C) 2008-2017 Michael Fink
//
/// \file VideoSearchView.hpp View to search for videos
//
#pragma once

#include "WebPageWindowEx.hpp"
#include <functional>

#define WM_INIT_VIEW WM_APP + 1

class VideoSearchViewUIHandler : public IDocHostUIHandlerDispatchImpl
{
public:
   void OnNewWindow3(const CString& /*cszURL*/, bool& bCancel)
   {
      bCancel = true; // cancel all newly opened windows
   }
};

class VideoSearchView :
   public WebPageWindowEx<VideoSearchView, VideoSearchViewUIHandler>
{
public:
   typedef WebPageWindowEx<VideoSearchView, VideoSearchViewUIHandler> BaseClass;

   void SearchVideo(const CString& cszText);

   BOOL PreTranslateMessage(MSG* pMsg);

   typedef std::function<void(const CString&, const CString&)> T_fnAddTitle;

   void SetAddCallback(T_fnAddTitle fnAddTitle = T_fnAddTitle())
   {
      m_fnAddTitle = fnAddTitle;
   }

   void OnBeforeNavigate2(const CString& /*cszURL*/, const CString& /*cszTargetFrameName*/, bool& bCancel);

   BEGIN_MSG_MAP(VideoSearchView)
      MESSAGE_HANDLER(WM_CREATE, OnCreate)
      MESSAGE_HANDLER(WM_INIT_VIEW, OnInitView)
      CHAIN_MSG_MAP(BaseClass)
   END_MSG_MAP()

   // Handler prototypes (uncomment arguments if needed):
   // LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
   // LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
   // LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

   LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
   LRESULT OnInitView(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

private:
   static void ReplaceHtmlEntities(CString& cszText);

private:
   T_fnAddTitle m_fnAddTitle;
};
