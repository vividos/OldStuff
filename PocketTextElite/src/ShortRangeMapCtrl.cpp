//
// PocketTextElite - port of Elite[TM] trading system
// Copyright (C) 2008-2009 Michael Fink
//
/// \file ShortRangeMapCtrl.cpp Short range map control
//

// includes
#include "stdafx.h"
#include "ShortRangeMapCtrl.hpp"

#include <algorithm>
#undef min
#undef max

void ShortRangeMapCtrlBase::UpdatePlanetInfos(const std::vector<PlanetInfo>& vecInfos)
{
   m_vecInfos.clear();
   m_vecInfos.assign(vecInfos.begin(), vecInfos.end());

   UpdateHitTestInfo();
}

LRESULT ShortRangeMapCtrlBase::OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   if (m_uiLightyearUnit == 0)
      return 0;

   CPaintDC dc(m_hWnd);

   // black pen
   CPen pen;
   pen.CreatePen(PS_SOLID, 1, RGB(0,0,0));

   // yellow brush
   CBrush br;
   br.CreateSolidBrush(RGB(255,255,0));

   HPEN hPenOld = dc.SelectPen(pen);
   HBRUSH hBrushOld = dc.SelectBrush(br);

   // draw circle, 7 ly radius
   double dRadius = m_uiRangeInLightYearTenths / 10.0;
   dc.Ellipse(
      m_ptCenter.x - int(m_uiLightyearUnit * dRadius), m_ptCenter.y -int( m_uiLightyearUnit * dRadius),
      m_ptCenter.x + int(m_uiLightyearUnit * dRadius), m_ptCenter.y +int( m_uiLightyearUnit * dRadius));

   // draw current planet
   if (!m_vecInfos.empty())
   {
      bool bSelected = m_vecInfos[0].PlanetIndex() == m_uiSelectedPlanetIndex;
      DrawPlanet(dc, m_ptCenter, bSelected, m_vecInfos[0].Name());

      // draw all remaining planets
      {
         int iLyCenterSystemX = m_vecInfos[0].XPos(),
            iLyCenterSystemY = m_vecInfos[0].YPos();

         for (size_t i=1, iMax=m_vecInfos.size(); i<iMax; i++)
         {
            int iLyDeltaX = m_vecInfos[i].XPos() - iLyCenterSystemX;
            int iLyDeltaY = m_vecInfos[i].YPos() - iLyCenterSystemY;

            CPoint ptCenter(m_ptCenter.x + int(0.4 * iLyDeltaX * m_uiLightyearUnit),
               m_ptCenter.y + int(0.4 * iLyDeltaY * m_uiLightyearUnit));

            bSelected = m_vecInfos[i].PlanetIndex() == m_uiSelectedPlanetIndex;
            DrawPlanet(dc, ptCenter, bSelected, m_vecInfos[i].Name());
         }
      }
   }

   // select previous pen and brush
   dc.SelectPen(hPenOld);
   dc.SelectBrush(hBrushOld);

   return 0;
}

void ShortRangeMapCtrlBase::DrawPlanet(CDC& dc, CPoint& pt, bool bSelected, LPCTSTR pszName)
{
   // draw cross when selected
   if (bSelected)
   {
      dc.MoveTo(pt.x - int(m_uiLightyearUnit * 1.1), pt.y);
      dc.LineTo(pt.x + int(m_uiLightyearUnit * 1.1), pt.y);

      dc.MoveTo(pt.x, pt.y - int(m_uiLightyearUnit * 1.1));
      dc.LineTo(pt.x, pt.y + int(m_uiLightyearUnit * 1.1));
   }

   // draw filled circle
   dc.Ellipse(
      pt.x - int(m_uiLightyearUnit * 0.4), pt.y - int(m_uiLightyearUnit * 0.4),
      pt.x + int(m_uiLightyearUnit * 0.4), pt.y + int(m_uiLightyearUnit * 0.4));

   // draw name
   CRect rc(pt.x, pt.y-20, pt.x+1000, pt.y);
   dc.DrawText(pszName, static_cast<int>(_tcslen(pszName)), rc, 0);
}

LRESULT ShortRangeMapCtrlBase::OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   CRect rc;
   GetClientRect(rc);

   // calculate center point
   m_ptCenter = rc.CenterPoint();

   // calculate lightyear unit
   int iRectSize = std::min(rc.Width(), rc.Height());
   m_uiLightyearUnit = iRectSize / 16; // the element should show at least 16 light years

   UpdateHitTestInfo();

   return 0;
}

LRESULT ShortRangeMapCtrlBase::OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
   CPoint pt(LOWORD(lParam), HIWORD(lParam));

   // search all hit test infos
   for (size_t i=0, iMax=m_vecHitTestInfo.size(); i<iMax; i++)
   {
      const SHitTestInfo& info = m_vecHitTestInfo[i];
      if (info.m_rcArea.PtInRect(pt))
      {
         // found new selection
         ::SendMessage(GetParent(), SM_SELECTED_PLANET, info.m_uiPlanetIndex, 0);
         break;
      }
   }

   return 0;
}

void ShortRangeMapCtrlBase::UpdateHitTestInfo()
{
   if (m_vecInfos.empty())
      return;

   m_vecHitTestInfo.clear();

   int iLyCenterSystemX = m_vecInfos[0].XPos(),
      iLyCenterSystemY = m_vecInfos[0].YPos();

   for (size_t i=0, iMax=m_vecInfos.size(); i<iMax; i++)
   {
      const PlanetInfo& planetInfo = m_vecInfos[i];

      int iLyDeltaX = planetInfo.XPos() - iLyCenterSystemX;
      int iLyDeltaY = planetInfo.YPos() - iLyCenterSystemY;

      CPoint pt(m_ptCenter.x + int(0.4 * iLyDeltaX * m_uiLightyearUnit),
         m_ptCenter.y + int(0.4 * iLyDeltaY * m_uiLightyearUnit));

      CRect rcHitArea(
         pt.x - int(m_uiLightyearUnit * 0.7), pt.y - int(m_uiLightyearUnit * 0.7),
         pt.x + int(m_uiLightyearUnit * 0.7), pt.y + int(m_uiLightyearUnit * 0.7));

      SHitTestInfo info;
      info.m_rcArea = rcHitArea;
      info.m_uiPlanetIndex = planetInfo.PlanetIndex();

      // add to list
      m_vecHitTestInfo.push_back(info);
   }
}
