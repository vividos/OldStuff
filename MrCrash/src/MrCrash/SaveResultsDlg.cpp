//
// MrCrash - crash info reporting tool
// Copyright (C) 2005-2014 Michael Fink
//
/// \file SaveResultsDlg.cpp Dialog to save result files
//

// includes
#include "stdafx.h"
#include "resource.h"
#include "SaveResultsDlg.hpp"

#pragma comment(lib, "shell32.lib") // for CFolderDialog

SaveResultsDlg::SaveResultsDlg(const std::vector<DebuggerResultEntry>& vecResults)
:m_vecResults(vecResults)
{
}

LRESULT SaveResultsDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
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

   // set up results list
   m_listResults.SetExtendedListViewStyle(LVS_EX_FULLROWSELECT);
   m_listResults.InsertColumn(0, _T("Titel"), LVCFMT_LEFT, 200);
   m_listResults.InsertColumn(1, _T("Filename"), LVCFMT_LEFT, 100);
   m_listResults.SetColumnWidth(1, LVSCW_AUTOSIZE_USEHEADER);

   // add results
   for (size_t i=0,iMax=m_vecResults.size(); i<iMax; i++)
   {
      const DebuggerResultEntry& entry = m_vecResults[i];

      int iItem = m_listResults.InsertItem(0, entry.m_cszTitle);
      m_listResults.SetItemText(iItem, 1, entry.m_cszFilename);
   }

   DlgResize_Init(true);

   return TRUE;
}

LRESULT SaveResultsDlg::OnExit(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   if (wID == IDOK)
   {
      // ask for folder to save
      CString cszFolderName;
      {
         CFolderDialog dlg(m_hWnd, _T("Select folder to store files..."),
            BIF_RETURNONLYFSDIRS | BIF_USENEWUI);
         if (IDOK != dlg.DoModal())
            return 0; // return without closing dialog

         cszFolderName = dlg.GetFolderPath();
         if (cszFolderName.Right(1) != _T("\\"))
            cszFolderName += _T("\\");
      }

      // copy all files to folder
      {
         for (size_t i=0,iMax=m_vecResults.size(); i<iMax; i++)
         {
            const DebuggerResultEntry& entry = m_vecResults[i];

            // get filename part
            int iPos = entry.m_cszFilename.ReverseFind(_T('\\'));
            CString cszFilenameOnly =
               iPos == -1 ? entry.m_cszFilename : entry.m_cszFilename.Mid(iPos+1);

            ::MoveFile(entry.m_cszFilename, cszFolderName + cszFilenameOnly);
         }
      }
   }

   EndDialog(wID);
   return 0;
}
