// RecorderFrame.cpp : implementation of the CRecorderFrame class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resourceppc.h"

#include "aboutdlg.h"
#include "RecorderView.h"
#include "RecorderFrame.h"

#define WM_SHELL_NOTIFY_MSG WM_APP + 1
#define IDR_SHELL_NOTIFY_ICON IDR_MAINFRAME


BOOL CRecorderFrame::PreTranslateMessage(MSG* pMsg)
{
	if(CFrameWindowImpl<CRecorderFrame>::PreTranslateMessage(pMsg))
		return TRUE; 

	return m_view.IsWindow() ? m_view.PreTranslateMessage(pMsg) : FALSE;
}

bool CRecorderFrame::AppHibernate( bool bHibernate)
{
	// Insert your code here or delete member if not relevant
	return bHibernate;
}

bool CRecorderFrame::AppNewInstance( LPCTSTR lpstrCmdLine)
{
	// Insert your code here or delete member if not relevant
	return false;
}

void CRecorderFrame::AppSave()
{
	CAppInfo info;
	bool bStatus = (UIGetState(ID_VIEW_STATUS_BAR) & UPDUI_CHECKED) == UPDUI_CHECKED;
	info.Save(bStatus, L"Status");
	// Insert your code here
}

BOOL CRecorderFrame::OnIdle()
{
	UIUpdateToolBar();
	UIUpdateStatusBar();
	return FALSE;
}

LRESULT CRecorderFrame::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CAppInfo info;

   // menu bar
	OSVERSIONINFO osvi;
	GetVersionEx(&osvi);
/*
	if (osvi.dwMajorVersion >= 5)
	   CreateSimpleCEMenuBar();
   else
*/
   {
   	CreateSimpleCEMenuBar(IDR_MAINFRAME, 0, IDR_MAINFRAME, 7);
      //CreateSimpleCEMenuBar(IDR_MAINFRAME, SHCMBF_EMPTYBAR);
   }
	UIAddToolBar(m_hWndCECommandBar);

	// StatusBar state restoration 
	bool bVisible = true;
	info.Restore(bVisible, L"Status");
	DWORD dwStyle = WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
	if (bVisible)
		dwStyle |= WS_VISIBLE;
	// StatusBar creation 
	CreateSimpleStatusBar(ATL_IDS_IDLEMESSAGE, dwStyle);
	UIAddStatusBar(m_hWndStatusBar);
	UISetCheck(ID_VIEW_STATUS_BAR, bVisible);

	m_hWndClient = m_view.Create(m_hWnd);

	// register object for message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->AddMessageFilter(this);
	pLoop->AddIdleHandler(this);

   // shell notify icon
   m_notifyIcon.SetCallback(this);
   m_notifyIcon.Create(m_hWnd, IDR_MAINFRAME, WM_SHELL_NOTIFY_MSG);

   HICON hNotifyIcon = ::LoadIcon(ModuleHelper::GetResourceInstance(),
      MAKEINTRESOURCE(IDR_MAINFRAME));
   m_notifyIcon.ShowIcon(hNotifyIcon);

   // start timer for battery manager
   unsigned int uiSeconds = App().Settings().GetUInt(Settings::TimerBatteryManager);
   ::SetTimer(m_hWnd, IDT_TIMER_BATTERY_MANAGER, uiSeconds * 1000, NULL);
   return 0;
}

LRESULT CRecorderFrame::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   m_notifyIcon.HideIcon();

   ::KillTimer(m_hWnd, IDT_TIMER_BATTERY_MANAGER);

   // end message loop
   PostQuitMessage(0);

   return 0;
}

LRESULT CRecorderFrame::OnTimer(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
{
   if (wParam == IDT_TIMER_BATTERY_MANAGER)
   {
      App().BatteryManager().Tick();

      // update values on form
      m_view.UpdateBatteryStatus(
         App().BatteryManager().BatteryCapacity(),
         static_cast<DWORD>(App().BatteryManager().RemainingTime().TotalSeconds()));

      ::SystemIdleTimerReset();
   }
   else
      bHandled = false;
   return 0;
}



LRESULT CRecorderFrame::OnHotkey(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   ATLTRACE(_T("WM_HOTKEY\n"));
   return 0;
}

LRESULT CRecorderFrame::OnKeyDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   ATLTRACE(_T("WM_KEYDOWN\n"));
   return 0;
}

LRESULT CRecorderFrame::OnKeyUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   ATLTRACE(_T("WM_KEYUP\n"));
   return 0;
}



LRESULT CRecorderFrame::OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   PostMessage(WM_CLOSE);
   return 0;
}

LRESULT CRecorderFrame::OnFileNew(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   // TODO: add code to initialize document

	return 0;
}

LRESULT CRecorderFrame::OnViewStatusBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	BOOL bVisible = !::IsWindowVisible(m_hWndStatusBar);
	::ShowWindow(m_hWndStatusBar, bVisible ? SW_SHOWNOACTIVATE : SW_HIDE);
	UISetCheck(ID_VIEW_STATUS_BAR, bVisible);
	UpdateLayout();
	return 0;
}

LRESULT CRecorderFrame::OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CAboutDlg dlg;
	dlg.DoModal();
	return 0;
}

void CRecorderFrame::OnActivateApplication()
{
   ATLTRACE(_T("OnActivateApplication\n"));
//   m_shellNotifyIcon.HideIcon();
   ShowWindow(SW_RESTORE);
   SetForegroundWindow(m_hWnd);
}

void CRecorderFrame::OnShowContextMenu()
{
}
