//
// Recorder - a GPS logger app for Windows Mobile
// Copyright (C) 2006-2019 Michael Fink
//
/// \file ShellNotifyIcon.cpp Shell notify icon implementation
//
#include "stdafx.h"
#include "ShellNotifyIcon.hpp"

#ifndef _WIN32_WCE
#  pragma comment(lib, "shell32.lib")
#endif

UINT CShellNotifyIcon::s_uiTaskbarCreated = ::RegisterWindowMessage(_T("TaskbarCreated"));

CShellNotifyIcon::CShellNotifyIcon()
   :m_bIconShown(false),
   m_pCallback(NULL),
   m_bInHandler(false)
{
   ZeroMemory(&m_nid, sizeof(m_nid));
   m_nid.cbSize = sizeof(NOTIFYICONDATA);
   m_nid.uID = static_cast<UINT>(-1);
   m_nid.hWnd = NULL;
}

CShellNotifyIcon::~CShellNotifyIcon()
{
   if (m_nid.hWnd != NULL)
      HideIcon();
}

void CShellNotifyIcon::Create(HWND hWndParent, UINT uTrayIconID, UINT uTaskbarCallbackMessage)
{
   ATLASSERT(uTaskbarCallbackMessage >= WM_APP);

   m_nid.hWnd = hWndParent;
   m_nid.uID = uTrayIconID;
   m_nid.uCallbackMessage = uTaskbarCallbackMessage;
}

void CShellNotifyIcon::ShowIcon(HICON hIcon)
{
   ATLASSERT(m_nid.hWnd != NULL);

   m_nid.uFlags = NIF_ICON | NIF_MESSAGE;
   m_nid.hIcon = hIcon;

   Shell_NotifyIcon(!m_bIconShown ? NIM_ADD : NIM_MODIFY, &m_nid);

   m_bIconShown = true;
}

void CShellNotifyIcon::HideIcon()
{
   ATLASSERT(m_nid.hWnd != NULL);

   m_nid.uFlags = 0;
   Shell_NotifyIcon(NIM_DELETE, &m_nid);

   m_bIconShown = false;
}

void CShellNotifyIcon::SetTooltipText(LPCTSTR pszText)
{
   ATLASSERT(m_nid.hWnd != NULL);

   m_nid.uFlags = NIF_TIP;

   unsigned int uiMax = sizeof(m_nid.szTip) / sizeof(*m_nid.szTip);
   _tcsncpy(m_nid.szTip, pszText, uiMax - 1);
   m_nid.szTip[uiMax - 1] = 0;

   Shell_NotifyIcon(NIM_MODIFY, &m_nid);
}

#ifndef _WIN32_WCE
/// Shows balloon tooltip that points to the shell notify icon.
/// \param pszText text for the balloon tooltip
/// \param pszCaption caption text for the tooltip; when set to NULL, caption is not shown
/// \param dwInfoFlags flags for the balloon tooltip. Currently the following flags can be passed:
///        - NIIF_NONE: don't show icon in balloon
///        - NIIF_INFO, NIIF_WARNING or NIIF_ERROR: set either one of these to set the appropriate icon
///        - NIIF_USER: show notify icon as balloon icon
///        - NIIF_NOSOUND: don't play sound normally played when balloon is shown
/// \param uTimeout timeout in seconds; use a value between 10 and 30 (the system ensures this range anyway).
void CShellNotifyIcon::ShowBalloonTooltip(LPCTSTR pszText, LPCTSTR pszCaption, DWORD dwInfoFlags, UINT uTimeout)
{
   ATLASSERT(pszText != NULL);
   ATLASSERT(uTimeout >= 10 && uTimeout <= 30);
   ATLASSERT(true == m_bIconShown);

   m_nid.uFlags = NIF_INFO;

   m_nid.uTimeout = uTimeout * 1000;
   m_nid.dwInfoFlags = dwInfoFlags;

   // balloon text
   unsigned int uiMax = sizeof(m_nid.szInfo) / sizeof(*m_nid.szInfo);
   _tcsncpy(m_nid.szInfo, pszText, uiMax - 1);
   m_nid.szInfo[uiMax - 1] = 0;

   // balloon title
   if (pszCaption == NULL)
      m_nid.szInfoTitle[0] = 0;
   else
   {
      uiMax = sizeof(m_nid.szInfoTitle) / sizeof(*m_nid.szInfoTitle);
      _tcsncpy(m_nid.szInfoTitle, pszCaption, uiMax - 1);
      m_nid.szInfoTitle[uiMax - 1] = 0;
   }

   Shell_NotifyIcon(NIM_MODIFY, &m_nid);

   // set to 0 again to prevent reusing the text in later calls
   m_nid.szInfo[0] = 0;
}
#endif // !_WIN32_WCE

void CShellNotifyIcon::ShowContextMenu(HMENU hMenu)
{
   ATLASSERT(m_bInHandler == true);

   HMENU hSubMenu = GetSubMenu(hMenu, 0);

   CPoint ptPos;
#ifdef _WIN32_WCE
   ptPos = CPoint(GetMessagePos());
#else
   GetCursorPos(&ptPos);
#endif

   ATLTRACE(_T("context menu at x=%u, y=%u\n"), ptPos.x, ptPos.y);

   // http://blogs.msdn.com/jeffdav/archive/2006/05/01/587785.aspx
   SetForegroundWindow(m_nid.hWnd);

#ifndef _WIN32_WCE
   DWORD dwFlags = 0;
#else
   DWORD dwFlags = TPM_LEFTALIGN;
#endif

   ::TrackPopupMenuEx(hSubMenu, dwFlags, ptPos.x, ptPos.y, m_nid.hWnd, NULL);

   // see Microsoft KB article 135788 for more
   PostMessage(m_nid.hWnd, WM_NULL, 0, 0);
}

LRESULT CShellNotifyIcon::OnTaskbarCreated(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   ATLTRACE(_T("TaskbarCreated\n"));

   // message gets sent when taskbar is recreated, so just add our icon when shown
   if (m_bIconShown)
   {
      m_nid.uFlags = NIF_ICON | NIF_MESSAGE;
      Shell_NotifyIcon(NIM_ADD, &m_nid);
   }

   return 0;
}

LRESULT CShellNotifyIcon::OnTaskbarContextMenu(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   ATLTRACE(_T("TaskbarContextMenu\n"));
   ATLASSERT(false);
   return 0;
}

LRESULT CShellNotifyIcon::OnTaskbarCallback(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
   ATLASSERT(m_nid.hWnd != NULL); // must be initialized, or why would we get messages then?

   // message for our icon?
   if (m_nid.uID != wParam)
   {
      bHandled = false;
      return 0;
   }

   switch (lParam)
   {
      // note: those messages are sent for pre-5.0 shell
   case WM_LBUTTONDOWN:
   case WM_LBUTTONDBLCLK:
      // tap-and-hold recognition on PocketPC
/*
#ifdef _WIN32_WCE
      {
         SHRGINFO shrgi = {0};
         shrgi.cbSize = sizeof(SHRGINFO);
         shrgi.hwndClient = m_nid.hWnd;
         shrgi.ptDown = CPoint(GetMessagePos());
         shrgi.dwFlags = SHRG_RETURNCMD;

         ATLTRACE(_T("check for gesture at x=%u, y=%u, pos=%08x\n"), shrgi.ptDown.x, shrgi.ptDown.y, GetMessagePos());

         if (GN_CONTEXTMENU == ::SHRecognizeGesture(&shrgi))
         {
            ATLTRACE(_T("SHRecognizeGesture returned GN_CONTEXTMENU\n"));
            m_bInHandler = true;
            if (m_pCallback != NULL)
               m_pCallback->OnShowContextMenu();
            m_bInHandler = false;
            break;
         }
      }
#endif
*/
      if (m_pCallback != NULL)
         m_pCallback->OnActivateApplication();
      break;

   case WM_RBUTTONDOWN:
   case WM_RBUTTONDBLCLK:
      m_bInHandler = true;
      if (m_pCallback != NULL)
         m_pCallback->OnShowContextMenu();
      m_bInHandler = false;
      break;

#ifndef _WIN32_WCE
   case NIN_KEYSELECT:
      ShowBalloonTooltip(_T("The key select"), _T("My Application"));
      break;

   case NIN_SELECT:
      ShowBalloonTooltip(_T("The select"), _T("My Application"));
      break;
#endif

   case WM_MOUSEMOVE:
   case WM_LBUTTONUP:
   case WM_RBUTTONUP:
      break;

#ifndef _WIN32_WCE
   case NIN_BALLOONSHOW:
   case NIN_BALLOONHIDE:
   case NIN_BALLOONTIMEOUT:
   case NIN_BALLOONUSERCLICK:
#endif
   default:
      ATLTRACE(_T("TaskbarCallback wParam=%08x lParam=%08x\n"), wParam, lParam);
      break;
   }

   return 0;
}
