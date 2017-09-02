//
// PocketTextElite - port of Elite[TM] trading system
// Copyright (C) 2008-2009 Michael Fink
//
/// \file ShortRangeMapView.cpp Short range map view
//

// includes
#include "stdafx.h"
#include "resource.h"
#include "IMainFrame.hpp"
#include "ShortRangeMapView.hpp"
#include "TextElite.hpp"

LRESULT ShortRangeMapView::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   CRect rc;
   CWindow(GetDlgItem(IDC_STATIC_MAP)).GetWindowRect(rc);
   ScreenToClient(&rc);

   m_mapCtrl.Create(m_hWnd, rc);

   DlgResize_Init(false, true);

   return 1;
}

LRESULT ShortRangeMapView::OnSelectedPlanet(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   m_uiSelectedPlanet = static_cast<unsigned int>(wParam);

   m_mainFrame.SelectPlanet(m_uiSelectedPlanet);

   // set new selection
   m_mapCtrl.SetSelectedPlanetIndex(m_uiSelectedPlanet);
   m_mapCtrl.Invalidate();

   // format text
   PlanetInfo planetSelected = TextElite::GetPlanetInfo(m_uiSelectedPlanet);

   CString cszText;
   cszText.Format(_T("Selected system: %s"), planetSelected.Name().GetString());
   m_mainFrame.ShowResponse(cszText.GetString(), true);

   // update text
   unsigned int uiLightYearsTenths =
      TextElite::CalcDistance(TextElite::GetCurrentPlanet(), m_uiSelectedPlanet);

   cszText.Format(_T("Selected system: %s (%u.%01u Light Years)"),
      planetSelected.Name().GetString(),
      uiLightYearsTenths / 10, uiLightYearsTenths % 10);

   SetDlgItemText(IDC_STATIC_SELECTED_PLANET, cszText);

   return 0;
}

LRESULT ShortRangeMapView::OnButtonJump(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   CString cszResponse;
   bool bRet = TextElite::Jump(m_uiSelectedPlanet, cszResponse);
   m_mainFrame.ShowResponse(cszResponse, bRet);

   if (bRet)
   {
      m_mainFrame.EnteredNewSystem();

      UpdateView();
   }

   return 0;
}

LRESULT ShortRangeMapView::OnButtonGalaxyJump(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   CString cszResponse;
   TextElite::GalaxyJump();
   UpdateView();

   return 0;
}

void ShortRangeMapView::UpdateView()
{
   // get list of planets and update control

   std::vector<PlanetInfo> vecInfos;

   TextElite::GetLocalPlanets(vecInfos);

   // search for main planet and swap with position 0
   unsigned int uiCurrentPlanet = TextElite::GetCurrentPlanet();
   for (size_t i=0, iMax=vecInfos.size(); i<iMax; i++)
   {
      if (vecInfos[i].PlanetIndex() == uiCurrentPlanet)
      {
         if (i != 0)
            std::swap(vecInfos[i], vecInfos[0]);

         break;
      }
   }

   m_mapCtrl.UpdatePlanetInfos(vecInfos);

   // set new range radius
   m_mapCtrl.SetCurrentRange(TextElite::GetFuelInTenth());

   // set new selection
   m_mapCtrl.SetSelectedPlanetIndex(m_uiSelectedPlanet);

   m_mapCtrl.Invalidate();
}
