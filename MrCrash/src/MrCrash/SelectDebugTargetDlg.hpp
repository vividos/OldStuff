//
// MrCrash - crash info reporting tool
// Copyright (C) 2005-2014 Michael Fink
//
/// \file SelectDebugTargetDlg.hpp Dialog to select debug target
//
#pragma once

enum EDebugType
{
   dtStartApplication=0,
   dtAttachProcess=1,
};

class SelectDebugTargetDlg :
   public CDialogImpl<SelectDebugTargetDlg>,
   public CWinDataExchange<SelectDebugTargetDlg>,
   public CDialogResize<SelectDebugTargetDlg>
{
public:
   SelectDebugTargetDlg();

   EDebugType GetDebugType() const throw() { return static_cast<EDebugType>(m_iSelectedType); }

   // debug type dtAttachProcess:

   DWORD GetProcessId() const throw() { return m_dwProcessId; }

   // debug type dtStartApplication:

   CString GetApplication() const throw() { return m_cszApplication; }
   CString GetCommandLine() const throw() { return m_cszCommandLine; }

   /// returns if report should always be generated
   bool GetAlwaysCollectReport() const throw() { return m_bAlwaysCollectReport != FALSE; }

   enum { IDD = IDD_SELECT_DEBUG_TARGET };

private:
   BEGIN_MSG_MAP(SelectDebugTargetDlg)
      CHAIN_MSG_MAP(CDialogResize<SelectDebugTargetDlg>)
      MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
      COMMAND_ID_HANDLER(IDOK, OnExit)
      COMMAND_ID_HANDLER(IDCANCEL, OnExit)
      COMMAND_ID_HANDLER(IDC_BUTTON_SELECT_APP, OnButtonSelectApp)
      COMMAND_ID_HANDLER(IDC_RADIO_TYPE1, OnClickedRadioButton)
      COMMAND_ID_HANDLER(IDC_RADIO_TYPE2, OnClickedRadioButton)
      COMMAND_ID_HANDLER(IDC_BUTTON_REFRESH, OnButtonRefreshProcessList)
   END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
// LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
// LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
// LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

   LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
   LRESULT OnExit(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
   LRESULT OnButtonSelectApp(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
   LRESULT OnClickedRadioButton(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
   LRESULT OnButtonRefreshProcessList(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

   BEGIN_DDX_MAP(SelectDebugTargetDlg)
      DDX_RADIO(IDC_RADIO_TYPE1, m_iSelectedType)
      DDX_CONTROL_HANDLE(IDC_BUTTON_SELECT_APP, m_btnSelectApp)
      DDX_CONTROL_HANDLE(IDC_LIST_PROCESSES, m_listProcesses)
      DDX_TEXT(IDC_EDIT_APP, m_cszApplication)
      DDX_TEXT(IDC_EDIT_CMDLINE, m_cszCommandLine)
      DDX_CHECK(IDC_CHECKBOX_ALWAYS_COLLECT_REPORT, m_bAlwaysCollectReport)
   END_DDX_MAP()

   BEGIN_DLGRESIZE_MAP(SelectDebugTargetDlg)
      DLGRESIZE_CONTROL(IDC_RADIO_TYPE1, DLSZ_SIZE_X)
      DLGRESIZE_CONTROL(IDC_RADIO_TYPE2, DLSZ_SIZE_X)
      DLGRESIZE_CONTROL(IDC_EDIT_APP, DLSZ_SIZE_X)
      DLGRESIZE_CONTROL(IDC_BUTTON_SELECT_APP, DLSZ_MOVE_X)
      DLGRESIZE_CONTROL(IDC_EDIT_CMDLINE, DLSZ_SIZE_X)
      DLGRESIZE_CONTROL(IDC_LIST_PROCESSES, DLSZ_SIZE_X | DLSZ_SIZE_Y)
      DLGRESIZE_CONTROL(IDCANCEL, DLSZ_MOVE_X | DLSZ_MOVE_Y)
      DLGRESIZE_CONTROL(IDOK, DLSZ_MOVE_X | DLSZ_MOVE_Y)
   END_DLGRESIZE_MAP()

private:
   /// refreshes process list
   void RefreshProcessList();

   /// updates enabled and disabled controls based on selected type
   void UpdateEnabledControl(bool bEnableType1, bool bEnableType2);

private:
   int m_iSelectedType;
   CButton m_btnSelectApp;
   CListViewCtrl m_listProcesses;

   DWORD m_dwProcessId;
   CString m_cszApplication;
   CString m_cszCommandLine;
   BOOL m_bAlwaysCollectReport;
};
