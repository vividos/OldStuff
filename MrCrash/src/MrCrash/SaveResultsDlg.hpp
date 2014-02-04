//
// MrCrash - crash info reporting tool
// Copyright (C) 2005-2014 Michael Fink
//
/// \file SaveResultsDlg.hpp Dialog to save result files
//
#pragma once

// includes
#include <vector>
#include "Debugger.hpp"

class SaveResultsDlg :
   public CDialogImpl<SaveResultsDlg>,
   public CWinDataExchange<SaveResultsDlg>,
   public CDialogResize<SaveResultsDlg>
{
public:
   SaveResultsDlg(const std::vector<DebuggerResultEntry>& vecResults);

   enum { IDD = IDD_SAVE_RESULTS };

private:
   BEGIN_MSG_MAP(SaveResultsDlg)
      CHAIN_MSG_MAP(CDialogResize<SaveResultsDlg>)
      MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
      COMMAND_ID_HANDLER(IDOK, OnExit)
      COMMAND_ID_HANDLER(IDCANCEL, OnExit)
   END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
// LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
// LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
// LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

   LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
   LRESULT OnExit(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

   BEGIN_DDX_MAP(SaveResultsDlg)
      DDX_CONTROL_HANDLE(IDC_LIST_RESULTS, m_listResults)
   END_DDX_MAP()

   BEGIN_DLGRESIZE_MAP(SaveResultsDlg)
      DLGRESIZE_CONTROL(IDC_STATIC_TEXT, DLSZ_SIZE_X)
      DLGRESIZE_CONTROL(IDOK, DLSZ_MOVE_X | DLSZ_MOVE_X)
      DLGRESIZE_CONTROL(IDCANCEL, DLSZ_MOVE_X | DLSZ_MOVE_X)
      DLGRESIZE_CONTROL(IDC_LIST_RESULTS, DLSZ_SIZE_X | DLSZ_SIZE_Y)
   END_DLGRESIZE_MAP()

private:
   const std::vector<DebuggerResultEntry>& m_vecResults;
   CListViewCtrl m_listResults;
};
