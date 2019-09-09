//
// Recorder - a GPS logger app for Windows Mobile
// Copyright (C) 2006-2019 Michael Fink
//
/// \file RecorderView.h Recorder view class
//
#pragma once

#include <atlddx.h>

class CApplication;

class CRecorderView :
   public CDialogImpl<CRecorderView>,
   public CWinDataExchange<CRecorderView>
{
public:
   CRecorderView(CApplication& app)
      :m_app(app),
      m_bAssignActive(false),
      m_bLogging(false)
   {
   }

   ~CRecorderView() throw();

   enum { IDD = IDD_RECORDER_FORM };

   BOOL PreTranslateMessage(MSG* pMsg);

   void UpdateBatteryStatus(BYTE bPercent, DWORD dwRemainingSeconds)
   {
      CString cszText;

      cszText.Format(_T("%u%%"), bPercent);
      SetDlgItemText(IDC_EDIT_BATTERY_PERCENT, cszText);

      cszText.Format(_T("%02u:%02u:%02u"),
         (dwRemainingSeconds / 3600) & 60,
         (dwRemainingSeconds / 60) & 60,
         dwRemainingSeconds % 60);
      SetDlgItemText(IDC_EDIT_BATTERY_REMAINING, cszText);
   }

   BEGIN_MSG_MAP(CRecorderView)
      MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
      MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
      COMMAND_HANDLER(IDC_BUTTON_BROWSE_FILENAME, BN_CLICKED, OnBtnClickedBrowseFilename)
      COMMAND_HANDLER(IDC_BUTTON_START_STOP, BN_CLICKED, OnBtnClickedStartStop)
      COMMAND_HANDLER(IDC_BUTTON_ASSIGN_KEY, BN_CLICKED, OnBtnClickedAssignKey)
      COMMAND_HANDLER(IDC_BUTTON_SWITCH_OFF, BN_CLICKED, OnBtnClickedSwitchOff)
   END_MSG_MAP()

   BEGIN_DDX_MAP(CRecorderView)
      DDX_CONTROL_HANDLE(IDC_EDIT_LOG_FILENAME, m_editLogFilename)
      DDX_CONTROL_HANDLE(IDC_BUTTON_START_STOP, m_btnStartStop)

      DDX_CONTROL_HANDLE(IDC_BUTTON_ASSIGN_KEY, m_btnAssignKey)
      DDX_CONTROL_HANDLE(IDC_BUTTON_SWITCH_OFF, m_btnSwitchOff)
      DDX_CONTROL_HANDLE(IDC_STATIC_ASSIGNED_KEY, m_stAssignedKey)
   END_DDX_MAP()

private:
   // Handler prototypes (uncomment arguments if needed):
   // LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
   // LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
   // LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

   LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
   LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
   LRESULT OnBtnClickedBrowseFilename(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
   LRESULT OnBtnClickedStartStop(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
   LRESULT OnBtnClickedAssignKey(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
   LRESULT OnBtnClickedSwitchOff(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

   void OnAssignedHardwareKey(UINT uiKey);
   void OnSwitchOnHardwareKey(UINT uiKey);

   void OnTestOutput(const CString& cszText);

private:
   CApplication& m_app;

   CEdit m_editLogFilename;
   CButton m_btnStartStop;
   bool m_bLogging;

   bool m_bAssignActive;
   CButton m_btnAssignKey;
   CButton m_btnSwitchOff;
   CStatic m_stAssignedKey;
};
