//
// PocketTextElite - port of Elite[TM] trading system
// Copyright (C) 2008-2009 Michael Fink
//
/// \file MainFrame.hpp Main frame
//
#pragma once

// includes
#include "IMainFrame.hpp"
#include "MarketView.hpp"
#include "PlanetInfoView.hpp"
#include "ShortRangeMapView.hpp"

#define WINDOW_MENU_POSITION   3

/// main frame window
class MainFrame :
   public CFrameWindowImpl<MainFrame>,
   public CUpdateUI<MainFrame>,
   public CMessageFilter,
   public CIdleHandler,
   public IMainFrame
{
public:
   MainFrame();
   virtual ~MainFrame() throw() {}

   DECLARE_FRAME_WND_CLASS(NULL, IDR_MAINFRAME)

#ifdef UNDER_CE
   CBottomTabView m_tabView;
#else
   CTabView m_tabView;
#endif

   MarketView m_marketView;
   PlanetInfoView m_planetInfoView;
   ShortRangeMapView m_shortRangeMapView;

   virtual BOOL PreTranslateMessage(MSG* pMsg);
   virtual BOOL OnIdle();

   virtual void ShowResponse(LPCTSTR pszText, bool bSuccess);
   virtual void SelectPlanet(unsigned int uiSelectedPlanet);
   virtual void EnteredNewSystem();

   BEGIN_UPDATE_UI_MAP(MainFrame)
   END_UPDATE_UI_MAP()

   BEGIN_MSG_MAP(MainFrame)
      MESSAGE_HANDLER(WM_CREATE, OnCreate)
      MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
      COMMAND_ID_HANDLER(ID_APP_EXIT, OnFileExit)
      COMMAND_ID_HANDLER(ID_APP_ABOUT, OnAppAbout)
      NOTIFY_CODE_HANDLER(TBVN_PAGEACTIVATED, OnTabViewPageActivated)
#ifndef UNDER_CE
      COMMAND_RANGE_HANDLER(ID_WINDOW_TABFIRST, ID_WINDOW_TABLAST, OnWindowActivate)
#endif
      CHAIN_MSG_MAP(CUpdateUI<MainFrame>)
      CHAIN_MSG_MAP(CFrameWindowImpl<MainFrame>)
   END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
// LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
// LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
// LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

   LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
   LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
   LRESULT OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
   LRESULT OnFileNew(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
   LRESULT OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
   LRESULT OnTabViewPageActivated(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/);
   LRESULT OnWindowActivate(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

   static HRESULT ActivatePreviousInstance(HINSTANCE hInstance);

private:
   unsigned int m_uiSelectedPlanet;
};
