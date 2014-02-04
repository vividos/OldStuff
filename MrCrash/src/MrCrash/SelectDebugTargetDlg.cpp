//
// MrCrash - crash info reporting tool
// Copyright (C) 2005-2014 Michael Fink
//
/// \file SelectDebugTargetDlg.cpp Dialog to select debug target
//

// includes
#include "stdafx.h"
#include "resource.h"
#include "SelectDebugTargetDlg.hpp"
#include <ulib/debug/ToolHelp.hpp>
#include <ulib/win32/ProcessInfo.hpp>

SelectDebugTargetDlg::SelectDebugTargetDlg()
:m_iSelectedType(0)
{
}

LRESULT SelectDebugTargetDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   DoDataExchange(DDX_LOAD);

   // center the dialog on the screen
   CenterWindow();

   // set icons
   HICON hIcon = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME),
      IMAGE_ICON, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON), LR_DEFAULTCOLOR);
   SetIcon(hIcon, TRUE);
   HICON hIconSmall = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME),
      IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);
   SetIcon(hIconSmall, FALSE);

   // set up process list
   m_listProcesses.SetExtendedListViewStyle(LVS_EX_FULLROWSELECT);
   m_listProcesses.InsertColumn(0, _T("Process"), LVCFMT_LEFT, 200);
   m_listProcesses.InsertColumn(1, _T("PID"), LVCFMT_LEFT, 70);
   m_listProcesses.InsertColumn(2, _T("User"));
   m_listProcesses.SetColumnWidth(2, LVSCW_AUTOSIZE_USEHEADER);

   // set up browse button
   HICON hIconBrowse = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDI_BROWSE),
      IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);
   m_btnSelectApp.SetIcon(hIconBrowse);

   // refresh dialog contents
   RefreshProcessList();
   UpdateEnabledControl(m_iSelectedType == 0, m_iSelectedType != 0);

   DlgResize_Init(true);

   return TRUE;
}

LRESULT SelectDebugTargetDlg::OnExit(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   if (wID == IDOK)
   {
      DoDataExchange(DDX_SAVE);

      // type 1 "attach to process" is used?
      if (m_iSelectedType == 1)
      {
         // get currently selected process id
         int iItem = m_listProcesses.GetSelectedIndex();
         if (iItem != -1)
         {
            m_dwProcessId = m_listProcesses.GetItemData(iItem);
            // check if it's our process id
            if (GetCurrentProcessId() == m_dwProcessId)
            {
               AtlMessageBox(m_hWnd, _T("MrCrash cannot debug itself!"), _T("MrCrash"));
               return 0; // return without ending dialog
            }
         }
         else
            ATLASSERT(false);
      }
   }

   EndDialog(wID);
   return 0;
}

LRESULT SelectDebugTargetDlg::OnButtonSelectApp(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   DoDataExchange(DDX_SAVE, IDC_EDIT_APP);

   CFileDialog dlg(TRUE, _T(".exe"), m_cszApplication, 0,
      _T("Applications (*.exe)\0*.exe\0All files (*.*)\0*.*\0\0"),
      m_hWnd);

   if (IDOK == dlg.DoModal())
      m_cszApplication = dlg.m_szFileName;

   DoDataExchange(DDX_LOAD, IDC_EDIT_APP);
   return 0;
}

LRESULT SelectDebugTargetDlg::OnClickedRadioButton(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   bool bEnableType1 = wID == IDC_RADIO_TYPE1;
   bool bEnableType2 = wID == IDC_RADIO_TYPE2;

   UpdateEnabledControl(bEnableType1, bEnableType2);

   if (bEnableType2)
      RefreshProcessList();

   return 0;
}

LRESULT SelectDebugTargetDlg::OnButtonRefreshProcessList(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   RefreshProcessList();
   m_listProcesses.SetFocus();
   return 0;
}

void SelectDebugTargetDlg::UpdateEnabledControl(bool bEnableType1, bool bEnableType2)
{
   // type 1 controls
   CWindow(GetDlgItem(IDC_STATIC_APP)).EnableWindow(bEnableType1);
   CWindow(GetDlgItem(IDC_EDIT_APP)).EnableWindow(bEnableType1);
   CWindow(GetDlgItem(IDC_STATIC_CMDLINE)).EnableWindow(bEnableType1);
   CWindow(GetDlgItem(IDC_EDIT_CMDLINE)).EnableWindow(bEnableType1);
   CWindow(GetDlgItem(IDC_BUTTON_SELECT_APP)).EnableWindow(bEnableType1);

   // type 2 controls
   CWindow(GetDlgItem(IDC_BUTTON_REFRESH)).EnableWindow(bEnableType2);
   CWindow(GetDlgItem(IDC_LIST_PROCESSES)).EnableWindow(bEnableType2);
}

void SelectDebugTargetDlg::RefreshProcessList()
{
   // get currently selected pid
   DWORD dwLastSelectedPid = 0;
   int iItem = m_listProcesses.GetSelectedIndex();

   if (iItem != -1)
      dwLastSelectedPid = m_listProcesses.GetItemData(iItem);

   m_listProcesses.SetRedraw(false);

   m_listProcesses.DeleteAllItems();

   Debug::Toolhelp::Snapshot ts(TH32CS_SNAPPROCESS | TH32CS_SNAPNOHEAPS);
   Debug::Toolhelp::ProcessList processList(ts);

   int iLastSelectedItem = -1;
   for (size_t i = 0; i< processList.GetCount(); i++)
   {
      const PROCESSENTRY32& pe = processList.GetEntry(i);

      int iItem = m_listProcesses.InsertItem(m_listProcesses.GetItemCount(), pe.szExeFile);
      m_listProcesses.SetItemData(iItem, pe.th32ProcessID);

      if (dwLastSelectedPid != 0 && dwLastSelectedPid == pe.th32ProcessID)
         iLastSelectedItem = iItem;

      CString cszText;
      cszText.Format(_T("%u"), pe.th32ProcessID);
      m_listProcesses.SetItemText(iItem, 1, cszText);

      // set user and domain
      Win32::ProcessInfo pi(pe.th32ProcessID);
      CString cszUser, cszDomain;
      if (pi.GetUserAndDomainName(cszUser, cszDomain))
         m_listProcesses.SetItemText(iItem, 2, cszUser + _T(" (") + cszDomain + _T(")"));
      else
         m_listProcesses.SetItemText(iItem, 2, _T("???"));
   }

   m_listProcesses.SetColumnWidth(2, LVSCW_AUTOSIZE);

   m_listProcesses.SetRedraw(true);

   // set last selected pid
   if (iLastSelectedItem != -1)
      m_listProcesses.SelectItem(iLastSelectedItem);
}
