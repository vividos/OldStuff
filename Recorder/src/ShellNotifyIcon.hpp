#pragma once

// includes
#include <shellapi.h>

/// callback interface for shell notify icon
class IShellNotifyIconCallback
{
public:
   /// dtor
   virtual ~IShellNotifyIconCallback() throw() {}

   /// called when the application is activated with a left-click on the icon
   virtual void OnActivateApplication() = 0;

   /// called when a context menu should be shown when user right-clicked on the icon
   virtual void OnShowContextMenu() = 0;
};

/// \brief shell notify icon
/// icon that sits in the notify area; the user can click on the icon
class CShellNotifyIcon
{
public:
   /// ctor
   CShellNotifyIcon();
   /// dtor
   virtual ~CShellNotifyIcon();

   /// sets callback
   void SetCallback(IShellNotifyIconCallback* pCallback){ m_pCallback = pCallback; }

   /// initialize shell notify icon
   void Create(HWND hWndParent, UINT uTrayIconID, UINT uTaskbarCallbackMessage = WM_APP + 1);

   /// shows icon in notification area
   void ShowIcon(HICON hIcon);

   /// hides icon
   void HideIcon();

   /// sets text for normal tooltip
   void SetTooltipText(LPCTSTR pszText);

#ifndef _WIN32_WCE
   /// shows a balloon tooltip
   void ShowBalloonTooltip(LPCTSTR pszText, LPCTSTR pszCaption = NULL,
      DWORD dwInfoFlags = NIIF_NONE, UINT uTimeout = 30);
#endif

   /// shows given context menu in the tooltip area
   void ShowContextMenu(HMENU hMenu);

   // message processing for MFC usage
#ifdef _MFC_VER
   BOOL PreTranslateMessage(MSG* pMsg)
   {
      BOOL bHandled = TRUE;
      if (pMsg->message == s_uiTaskbarCreated)
         OnTaskbarCreated(pMsg->message, pMsg->wParam, pMsg->lParam, bHandled);
      else if (pMsg->message == m_nid.uCallbackMessage)
         OnTaskbarCallback(pMsg->message, pMsg->wParam, pMsg->lParam, bHandled);
      else if (pMsg->message == WM_CONTEXTMENU)
         OnTaskbarContextMenu(pMsg->message, pMsg->wParam, pMsg->lParam, bHandled);
      else
         bHandled = FALSE;
      return bHandled;
   }
#endif

protected:
   // message map for ATL/WTL usage
#ifdef __ATLWIN_H__
   BEGIN_MSG_MAP(CShellNotifyIcon)
      MESSAGE_HANDLER(s_uiTaskbarCreated, OnTaskbarCreated)
      MESSAGE_HANDLER(m_nid.uCallbackMessage, OnTaskbarCallback)
      MESSAGE_HANDLER(WM_CONTEXTMENU, OnTaskbarContextMenu)
   END_MSG_MAP()
#endif // __ATLWIN_H__

protected:
   LRESULT OnTaskbarCreated(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
   LRESULT OnTaskbarCallback(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
   LRESULT OnTaskbarContextMenu(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

private:
   NOTIFYICONDATA m_nid;
   static UINT s_uiTaskbarCreated;
   bool m_bIconShown;
   IShellNotifyIconCallback* m_pCallback;
   bool m_bInHandler;
};
