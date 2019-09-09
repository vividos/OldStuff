//
// Recorder - a GPS logger app for Windows Mobile
// Copyright (C) 2006-2019 Michael Fink
//
/// \file RecorderView.h Recorder view class implementation
//
#pragma once

#include "stdafx.h"
#include "resourceppc.h"
#include "RecorderView.h"
#include "Application.hpp"
#include <boost/bind.hpp>

CRecorderView::~CRecorderView() throw()
{
   // stop assigning key
   m_app.DisplayOffManager().StopAssign();
   // stop switching off display
   m_app.DisplayOffManager().SwitchOn();
}

BOOL CRecorderView::PreTranslateMessage(MSG* pMsg)
{
   return IsDialogMessage(pMsg);
}

LRESULT CRecorderView::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
   DoDataExchange(DDX_LOAD);
   m_btnAssignKey.EnableWindow(TRUE);
   m_btnSwitchOff.EnableWindow(m_app.DisplayOffManager().AssignedKey() == 0 ? FALSE : TRUE);

   m_editLogFilename.SetWindowText(_T("\\testlog.txt"));

   return 1;
}

LRESULT CRecorderView::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   m_app.Receiver().SetInputCheckCallback();
   return 0;
}

#include <atldlgs.h>

LRESULT CRecorderView::OnBtnClickedBrowseFilename(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   DoDataExchange(DDX_SAVE, IDC_EDIT_LOG_FILENAME);

   CString cszFilename;
   m_editLogFilename.GetWindowText(cszFilename);

   WTL::CFileDialog fileDlg(FALSE, // save file
      _T("txt"),
      cszFilename,
      OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
      _T("Text files (*.txt)\0*.txt\0All files (*.*)\0*.*\0\0"),
      m_hWnd);
   fileDlg.m_ofn.lStructSize = sizeof(OPENFILENAME);

   if (IDOK == fileDlg.DoModal())
   {
      m_editLogFilename.SetWindowText(fileDlg.m_ofn.lpstrFile);

      DoDataExchange(DDX_LOAD, IDC_EDIT_LOG_FILENAME);
   }

   return 0;
}

LRESULT CRecorderView::OnBtnClickedStartStop(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   if (m_bLogging)
   {
      // already running: stop logging
      m_app.CloseNMEALog();
      m_bLogging = false;
      m_btnStartStop.SetWindowText(_T("Start"));

      m_app.Receiver().Stop();
   }
   else
   {
      // not running yet; get filename
      DoDataExchange(DDX_SAVE, IDC_EDIT_LOG_FILENAME);

      CString cszFilename;
      m_editLogFilename.GetWindowText(cszFilename);

      // open log file
      m_app.AppendNMEALog(cszFilename);

      m_bLogging = true;
      m_btnStartStop.SetWindowText(_T("Stop"));

      m_app.Receiver().SetInputCheckCallback(boost::bind(&CRecorderView::OnTestOutput, this, _1));

      m_app.Receiver().Start();
   }
   return 0;
}

LRESULT CRecorderView::OnBtnClickedAssignKey(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   if (!m_bAssignActive)
   {
      m_bAssignActive = true;

      // starting assigning a key
      T_fnOnHardwareKey fnOnHardwareKey = boost::bind(&CRecorderView::OnAssignedHardwareKey, this, _1);
      m_app.DisplayOffManager().AssignKey(fnOnHardwareKey);

      m_btnAssignKey.SetWindowText(_T("Cancel assign"));
   }
   else
   {
      // stopping assigning a key
      m_bAssignActive = false;
      m_app.DisplayOffManager().StopAssign();

      m_btnAssignKey.SetWindowText(_T("Assign key"));
   }
   return 0;
}

void CRecorderView::OnAssignedHardwareKey(UINT uiKey)
{
   if (uiKey == 0)
      return; // no key was assigned

   m_btnSwitchOff.EnableWindow(TRUE);

   CString cszKeyName = m_app.DisplayOffManager().GetKeyName(uiKey);

   m_stAssignedKey.SetWindowText(_T("Key: ") + cszKeyName);

   m_btnAssignKey.SetWindowText(_T("Assign key"));

   m_app.DisplayOffManager().StopAssign();
}

void CRecorderView::OnSwitchOnHardwareKey(UINT uiKey)
{
   /*
   // If we would stop the hardware key manager at this point, it would
   // somehow pass on the key up to the OS, and the default action for the
   // key would be carried out. So simulate a click on the "cancel" button
   // here.
   PostMessage(WM_COMMAND,
      MAKEWPARAM(IDC_BUTTON_CANCEL, BN_CLICKED),
      (LPARAM)m_hWnd);
   */
}

LRESULT CRecorderView::OnBtnClickedSwitchOff(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   if (m_app.DisplayOffManager().AssignedKey() == 0)
      return 0; // do nothing; no key assigned yet

   // switch off display
   T_fnOnHardwareKey fnOnHardwareKey = boost::bind(&CRecorderView::OnSwitchOnHardwareKey, this, _1);
   m_app.DisplayOffManager().SwitchOff(fnOnHardwareKey);

   return 0;
}

void CRecorderView::OnTestOutput(const CString& cszText)
{
   GetDlgItem(IDC_EDIT_TEST_OUTPUT).SetWindowText(cszText);
}
