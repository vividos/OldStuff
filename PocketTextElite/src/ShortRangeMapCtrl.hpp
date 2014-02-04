//
// PocketTextElite - port of Elite[TM] trading system
// Copyright (C) 2008-2009 Michael Fink
//
/// \file ShortRangeMapCtrl.hpp Short range map control
//
#pragma once

// includes
#include "PlanetInfo.hpp"
#include <vector>

/// message to signal parent that selected parent has changed; new planet number in wParam
#define SM_SELECTED_PLANET WM_APP+1

/// base class for the short range map control
/// \note the control assumes that the first planet in the list is the current planet
class ShortRangeMapCtrlBase: public ATL::CWindow
{
public:
   ShortRangeMapCtrlBase()
      :m_uiLightyearUnit(0),
       m_uiRangeInLightYearTenths(70)
   {
   }

   void SetSelectedPlanetIndex(unsigned int uiSelectedPlanetIndex){ m_uiSelectedPlanetIndex = uiSelectedPlanetIndex; }
   void SetCurrentRange(unsigned int uiRangeInLightYearTenths){ m_uiRangeInLightYearTenths = uiRangeInLightYearTenths; }

   LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
   LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
   LRESULT OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

   void UpdatePlanetInfos(const std::vector<PlanetInfo>& vecInfos);

private:
   void DrawPlanet(CDC& dc, CPoint& pt, bool bSelected, LPCTSTR pszName);

   void UpdateHitTestInfo();

private:
   CPoint m_ptCenter;
   unsigned int m_uiLightyearUnit;
   unsigned int m_uiSelectedPlanetIndex;
   unsigned int m_uiRangeInLightYearTenths;

   std::vector<PlanetInfo> m_vecInfos;

   struct SHitTestInfo
   {
      unsigned int m_uiPlanetIndex;
      CRect m_rcArea;
   };

   std::vector<SHitTestInfo> m_vecHitTestInfo;
};

/// implementation class for the short range map control
template <class T, class TBase = ShortRangeMapCtrlBase, class TWinTraits = ATL::CControlWinTraits>
class ATL_NO_VTABLE ShortRangeMapCtrlImpl: public ATL::CWindowImpl<T, TBase, TWinTraits>
{
public:
   ShortRangeMapCtrlImpl(){}

private:
   BEGIN_MSG_MAP(ShortRangeMapCtrlImpl)
      MESSAGE_HANDLER(WM_PAINT, OnPaint)
      MESSAGE_HANDLER(WM_SIZE, OnSize)
      MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
   END_MSG_MAP()
};

/// short range map control
class ShortRangeMapCtrl: public ShortRangeMapCtrlImpl<ShortRangeMapCtrl>
{
public:
   DECLARE_WND_CLASS_EX(_T("PocketTextElite_ShortRangeMapCtrl"), 0, COLOR_APPWORKSPACE)
};
