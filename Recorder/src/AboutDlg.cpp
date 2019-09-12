//
// Recorder - a GPS logger app for Windows Mobile
// Copyright (C) 2006-2019 Michael Fink
//
/// \file AboutDlg.h About dialog
//
#include "stdafx.h"
#include "resourceppc.h"
#include "aboutdlg.h"

LRESULT CAboutDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
#ifdef _WIN32_WCE
   AtlCreateEmptyMenuBar(m_hWnd);
#endif

   return bHandled = FALSE;
}
