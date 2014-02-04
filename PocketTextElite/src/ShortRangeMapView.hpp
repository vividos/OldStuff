//
// PocketTextElite - port of Elite[TM] trading system
// Copyright (C) 2008-2009 Michael Fink
//
/// \file ShortRangeMapView.hpp Short range map view
//
#pragma once

// includes
#include "ShortRangeMapCtrl.hpp"

// forward references
class IMainFrame;

/// view for short range map
class ShortRangeMapView:
   public CDialogImpl<ShortRangeMapView>,
   public CDialogResize<ShortRangeMapView>,
   public CWinDataExchange<ShortRangeMapView>
{
public:
   ShortRangeMapView(IMainFrame& mainFrame)
      :m_mainFrame(mainFrame)
   {
   }

   enum { IDD = IDD_SHORTRANGEMAP_FORM };

   BOOL PreTranslateMessage(MSG* pMsg)
   {
      return CWindow::IsDialogMessage(pMsg);
   }

   void SetSelectedPlanet(unsigned int uiSelectedPlanet){ m_uiSelectedPlanet = uiSelectedPlanet; }

   void UpdateView();

private:
   friend class CDialogResize<ShortRangeMapView>;

   BEGIN_DDX_MAP(ShortRangeMapView)
      DDX_CONTROL(IDC_STATIC_MAP, m_mapCtrl)
   END_DDX_MAP()

   BEGIN_DLGRESIZE_MAP(CMarketView)
      DLGRESIZE_CONTROL(IDC_STATIC_MAP, DLSZ_SIZE_X | DLSZ_SIZE_Y)
      DLGRESIZE_CONTROL(IDC_STATIC_SELECTED_PLANET, DLSZ_MOVE_Y)
      DLGRESIZE_CONTROL(IDC_BUTTON_JUMP, DLSZ_MOVE_Y)
      DLGRESIZE_CONTROL(IDC_BUTTON_GALAXY_JUMP, DLSZ_MOVE_Y)
   END_DLGRESIZE_MAP()

   BEGIN_MSG_MAP(ShortRangeMapView)
      MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
      MESSAGE_HANDLER(SM_SELECTED_PLANET, OnSelectedPlanet)
      COMMAND_HANDLER(IDC_BUTTON_JUMP, BN_CLICKED, OnButtonJump)
      COMMAND_HANDLER(IDC_BUTTON_GALAXY_JUMP, BN_CLICKED, OnButtonGalaxyJump)
      CHAIN_MSG_MAP(CDialogResize<ShortRangeMapView>)
   END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
// LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
// LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
// LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

   LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
   LRESULT OnSelectedPlanet(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
   LRESULT OnButtonJump(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
   LRESULT OnButtonGalaxyJump(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

private:
   IMainFrame& m_mainFrame;

   /// currently selected planet; may differ from current
   unsigned int m_uiSelectedPlanet;

   ShortRangeMapCtrl m_mapCtrl;
};
