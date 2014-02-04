//
// PocketTextElite - port of Elite[TM] trading system
// Copyright (C) 2008-2009 Michael Fink
//
/// \file PlanetInfoView.hpp Planet info view
//
#pragma once

/// view for planet infos
class PlanetInfoView :
   public CDialogImpl<PlanetInfoView>,
   public CDialogResize<PlanetInfoView>
{
public:
   enum { IDD = IDD_PLANETINFO_FORM };

   BOOL PreTranslateMessage(MSG* pMsg)
   {
      return CWindow::IsDialogMessage(pMsg);
   }

   void SetSelectedPlanet(unsigned int uiSelectedPlanet){ m_uiSelectedPlanet = uiSelectedPlanet; }

   void UpdateView();

private:
   friend class CDialogResize<PlanetInfoView>;

   BEGIN_DLGRESIZE_MAP(PlanetInfoView)
      DLGRESIZE_CONTROL(IDC_STATIC_PLANET_INFO, DLSZ_SIZE_X | DLSZ_SIZE_Y)
   END_DLGRESIZE_MAP()

   BEGIN_MSG_MAP(PlanetInfoView)
      MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
      CHAIN_MSG_MAP(CDialogResize<PlanetInfoView>)
   END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
// LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
// LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
// LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

   LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

private:
   /// currently selected planet; may differ from current
   unsigned int m_uiSelectedPlanet;
};
