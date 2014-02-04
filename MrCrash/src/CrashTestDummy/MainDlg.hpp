//
// MrCrash - crash info reporting tool
// Copyright (C) 2005-2014 Michael Fink
//
/// \file MainDlg.hpp Main dialog
//
#pragma once

/// main crash dummy dialog
class MainDlg : public CDialogImpl<MainDlg>
{
public:
   /// dialog id
   enum { IDD = IDD_MAINDLG };

   BEGIN_MSG_MAP(MainDlg)
      MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
      COMMAND_ID_HANDLER(IDOK, OnOK)
      COMMAND_ID_HANDLER(IDCANCEL, OnOK)
      COMMAND_ID_HANDLER(IDC_BUTTON_CRASH1, OnBnClickedButtonCrash1)
      COMMAND_ID_HANDLER(IDC_BUTTON_CRASH2, OnBnClickedButtonCrash2)
      COMMAND_ID_HANDLER(IDC_BUTTON_CRASH3, OnBnClickedButtonCrash3)
      COMMAND_ID_HANDLER(IDC_BUTTON_CRASH4, OnBnClickedButtonCrash4)
      COMMAND_ID_HANDLER(IDC_BUTTON_CRASH5, OnBnClickedButtonCrash5)
      COMMAND_ID_HANDLER(IDC_BUTTON_CRASH6, OnBnClickedButtonCrash6)
      COMMAND_ID_HANDLER(IDC_BUTTON_CRASH7, OnBnClickedButtonCrash7)
      COMMAND_ID_HANDLER(IDC_BUTTON_CRASH8, OnBnClickedButtonCrash8)
   END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
// LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
// LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
// LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

   LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
   LRESULT OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
   LRESULT OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
   LRESULT OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

private:
   LRESULT OnBnClickedButtonCrash1(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/){ CrashDebugBreak(); return 0; }
   LRESULT OnBnClickedButtonCrash2(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/){ CrashStackOverflow(); return 0; }
   LRESULT OnBnClickedButtonCrash3(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/){ CrashAccessViolationRead(); return 0; }
   LRESULT OnBnClickedButtonCrash4(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/){ CrashAccessViolationWrite(); return 0; }
   LRESULT OnBnClickedButtonCrash5(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/){ CrashOutputDebugString(); return 0; }
   LRESULT OnBnClickedButtonCrash6(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/){ CrashLoadFreeLibrary(); return 0; }
   LRESULT OnBnClickedButtonCrash7(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/){ CrashCreateProcess(); return 0; }
   LRESULT OnBnClickedButtonCrash8(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/){ CrashCreateThread(); return 0; }

   void CrashDebugBreak();
   void CrashStackOverflow();
   void CrashAccessViolationRead();
   void CrashAccessViolationWrite();
   void CrashOutputDebugString();
   void CrashLoadFreeLibrary();
   void CrashCreateProcess();
   void CrashCreateThread();
};
