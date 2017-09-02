//
// PartyVideoDJ - YouTube crossfading app
// Copyright (C) 2008-2017 Michael Fink
//
/// \file ControlBarForm.hpp Form for upper control bar
//
#pragma once

#include <functional>

class ControlBarForm :
   public CDialogImpl<ControlBarForm>,
   public CDialogResize<ControlBarForm>,
   public CWinDataExchange<ControlBarForm>
{
public:
   typedef std::function<void(const CString&)> T_fnOnSearchCallback;

   ControlBarForm() {}

   enum { IDD = IDD_CONTROLBAR_FORM };

   BOOL PreTranslateMessage(MSG* pMsg)
   {
      if (pMsg->message == WM_KEYDOWN &&
         pMsg->wParam == VK_RETURN &&
         pMsg->hwnd == m_ecSearchText.m_hWnd)
      {
         if (m_fnOnSearchCallback != NULL)
         {
            CString cszText;
            int iLen = m_ecSearchText.GetWindowTextLength();

            m_ecSearchText.GetWindowText(cszText.GetBuffer(iLen + 1), iLen + 1);
            cszText.ReleaseBuffer();

            m_fnOnSearchCallback(cszText);
         }
         return TRUE;
      }

      return CWindow::IsDialogMessage(pMsg);
   }

   CString GetSearchText() const
   {
      UINT uiLen = m_ecSearchText.GetWindowTextLength();

      CString cszText;
      m_ecSearchText.GetWindowText(cszText.GetBuffer(uiLen), uiLen);
      cszText.ReleaseBuffer();

      return cszText;
   }

   bool IsButtonPlaylistPressed() const
   {
      return false;
   }

   void SetStartPauseButton(bool bPaused)
   {
      m_btnStartPause.SetWindowText(bPaused ? _T("&Play") : _T("&Pause"));
   }

   void SetPlaylistButton(bool bPressed)
   {
      m_btnPlaylist.SetCheck(bPressed ? BST_CHECKED : BST_UNCHECKED);
   }

   void SetSearchButton(bool bPressed)
   {
      m_btnSearch.SetCheck(bPressed ? BST_CHECKED : BST_UNCHECKED);
   }

   void SetSearchCallback(T_fnOnSearchCallback fnOnSearchCallback = T_fnOnSearchCallback())
   {
      m_fnOnSearchCallback = fnOnSearchCallback;
   }

private:
   BEGIN_DDX_MAP(ControlBarForm)
      DDX_CONTROL_HANDLE(IDC_BUTTON_START_PAUSE, m_btnStartPause)
      DDX_CONTROL_HANDLE(IDC_BUTTON_SEARCH, m_btnSearch)
      DDX_CONTROL_HANDLE(IDC_EDIT_SEARCH, m_ecSearchText)
      DDX_CONTROL_HANDLE(IDC_BUTTON_PLAYLIST, m_btnPlaylist)
   END_DDX_MAP()

   BEGIN_DLGRESIZE_MAP(ControlBarForm)
      DLGRESIZE_CONTROL(IDC_EDIT_SEARCH, DLSZ_SIZE_X)
      DLGRESIZE_CONTROL(IDC_BUTTON_PLAYLIST, DLSZ_MOVE_X)
   END_DLGRESIZE_MAP()

   friend CDialogResize<ControlBarForm>;

   BEGIN_MSG_MAP(ControlBarForm)
      MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
      if (uMsg == WM_COMMAND) // route command messages to parent
         ::SendMessage(GetParent(), uMsg, wParam, lParam);
   CHAIN_MSG_MAP(CDialogResize<ControlBarForm>)
      REFLECT_NOTIFICATIONS()
   END_MSG_MAP()

   // Handler prototypes (uncomment arguments if needed):
   // LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
   // LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
   // LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

   LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
   {
      // create font for buttons and edit control
      HFONT hFont = AtlGetDefaultGuiFont();

      LOGFONT lf = { 0 };
      ATLVERIFY(::GetObject(hFont, sizeof(LOGFONT), &lf) == sizeof(LOGFONT));

      lf.lfWeight = FW_BOLD;
      lf.lfHeight = 28;
      m_hFontBig = ::CreateFontIndirect(&lf);

      DoDataExchange(DDX_LOAD);
      DlgResize_Init(false, false);

      CWindow(GetDlgItem(IDC_BUTTON_START_PAUSE)).SetFont(m_hFontBig);
      CWindow(GetDlgItem(IDC_BUTTON_NEXT)).SetFont(m_hFontBig);
      CWindow(GetDlgItem(IDC_BUTTON_SEARCH)).SetFont(m_hFontBig);
      CWindow(GetDlgItem(IDC_EDIT_SEARCH)).SetFont(m_hFontBig);
      CWindow(GetDlgItem(IDC_BUTTON_PLAYLIST)).SetFont(m_hFontBig);

      return 1;
   }

private:
   T_fnOnSearchCallback m_fnOnSearchCallback;

   CButton m_btnStartPause;
   CButton m_btnSearch;
   CButton m_btnPlaylist;
   CEdit m_ecSearchText;
   CFontHandle m_hFontBig;
};
