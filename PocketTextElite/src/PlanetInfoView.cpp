//
// PocketTextElite - port of Elite[TM] trading system
// Copyright (C) 2008-2009 Michael Fink
//
/// \file PlanetInfoView.cpp Planet info view
//

// includes
#include "stdafx.h"
#include "resource.h"
#include "PlanetInfoView.hpp"
#include "TextElite.hpp"

LRESULT PlanetInfoView::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   DlgResize_Init(false, true);

   return 1;
}

void PlanetInfoView::UpdateView()
{
   CString cszText = TextElite::GetInfo(m_uiSelectedPlanet);

   SetDlgItemText(IDC_STATIC_PLANET_INFO, cszText);
}
