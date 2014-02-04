//
// PocketTextElite - port of Elite[TM] trading system
// Copyright (C) 2008-2009 Michael Fink
//
/// \file MainFrame.cpp Main frame
//

// includes
#include "stdafx.h"
#include "resource.h"
#include "AboutDlg.hpp"
#include "MainFrame.hpp"
#include "TextElite.hpp"

MainFrame::MainFrame()
:m_marketView(*this),
 m_shortRangeMapView(*this)
{
   TextElite::Init();
}

BOOL MainFrame::PreTranslateMessage(MSG* pMsg)
{
   if(CFrameWindowImpl<MainFrame>::PreTranslateMessage(pMsg))
      return TRUE;

   return m_tabView.PreTranslateMessage(pMsg);
}

BOOL MainFrame::OnIdle()
{
   return FALSE;
}

void MainFrame::ShowResponse(LPCTSTR pszText, bool bSuccess)
{
#ifdef UNDER_CE
   if (!bSuccess)
      MessageBox(pszText, _T("Pocket Text Elite"));
#else
   // since on Win32 we have a status bar, show the response there
   ::SetWindowText(m_hWndStatusBar, pszText);
   bSuccess;
#endif
}

void MainFrame::SelectPlanet(unsigned int uiSelectedPlanet)
{
   m_uiSelectedPlanet = uiSelectedPlanet;

   m_marketView.SetSelectedPlanet(m_uiSelectedPlanet);
}

void MainFrame::EnteredNewSystem()
{
   m_uiSelectedPlanet = TextElite::GetCurrentPlanet();
   m_marketView.SetSelectedPlanet(m_uiSelectedPlanet);
}

LRESULT MainFrame::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   EnteredNewSystem();

#ifdef UNDER_CE
   CreateSimpleCEMenuBar(IDR_MAINFRAME, SHCMBF_HMENU);

   // create tab view
   m_hWndClient = m_tabView.Create(m_hWnd, rcDefault);
#else
   CreateSimpleToolBar();

   CreateSimpleStatusBar();

   CRect rcWindow = rcDefault; // TODO
   m_hWndClient = m_tabView.Create(m_hWnd, rcWindow, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, WS_EX_CLIENTEDGE);

//   CMenuHandle menuMain = GetMenu();
//   m_tabView.SetWindowMenu(menuMain.GetSubMenu(WINDOW_MENU_POSITION));
#endif

   // create tab windows
   m_marketView.Create(m_tabView);
   m_planetInfoView.Create(m_tabView);
   m_shortRangeMapView.Create(m_tabView);

   m_tabView.AddPage(m_shortRangeMapView, _T("Local Map"));
   m_tabView.AddPage(m_marketView, _T("Market"));
   m_tabView.AddPage(m_planetInfoView, _T("Planet Info"));
   m_tabView.SetActivePage(0);

   // register object for message filtering and idle updates
   CMessageLoop* pLoop = _Module.GetMessageLoop();
   ATLASSERT(pLoop != NULL);
   pLoop->AddMessageFilter(this);
   pLoop->AddIdleHandler(this);

   return 0;
}

LRESULT MainFrame::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
   // unregister message filtering and idle updates
   CMessageLoop* pLoop = _Module.GetMessageLoop();
   ATLASSERT(pLoop != NULL);
   pLoop->RemoveMessageFilter(this);
   pLoop->RemoveIdleHandler(this);

   bHandled = FALSE;
   return 1;
}

LRESULT MainFrame::OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   m_tabView.DestroyWindow();

   PostMessage(WM_CLOSE);
   return 0;
}

LRESULT MainFrame::OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   AboutDlg dlg;
   dlg.DoModal();
   return 0;
}

LRESULT MainFrame::OnTabViewPageActivated(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/)
{
   int iPage = static_cast<int>(pnmh->idFrom);
   if (iPage == -1)
      return 0;

   HWND hWndPage = m_tabView.GetPageHWND(iPage);

   if (hWndPage == m_marketView.m_hWnd)
   {
      m_marketView.SetSelectedPlanet(m_uiSelectedPlanet);
      m_marketView.UpdateView();
   }
   else
   if (hWndPage == m_planetInfoView.m_hWnd)
   {
      m_planetInfoView.SetSelectedPlanet(m_uiSelectedPlanet);
      m_planetInfoView.UpdateView();
   }
   else
   if (hWndPage == m_shortRangeMapView.m_hWnd)
   {
      m_shortRangeMapView.SetSelectedPlanet(m_uiSelectedPlanet);
      m_shortRangeMapView.UpdateView();
   }

   return 0;
}

#ifndef UNDER_CE
LRESULT MainFrame::OnWindowActivate(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   int iPage = wID - ID_WINDOW_TABFIRST;
   m_tabView.SetActivePage(iPage);

   return 0;
}
#endif

HRESULT MainFrame::ActivatePreviousInstance(HINSTANCE hInstance)
{
   hInstance;

#ifdef UNDER_CE
   CFrameWndClassInfo& classInfo = MainFrame::GetWndClassInfo();

   int nRet = ::LoadString(hInstance, IDR_MAINFRAME, classInfo.m_szAutoName, sizeof(classInfo.m_szAutoName)/sizeof(classInfo.m_szAutoName[0]));
   ATLASSERT(0 != nRet);

   classInfo.m_wc.lpszClassName = classInfo.m_szAutoName;

   const TCHAR* pszClass = classInfo.m_wc.lpszClassName;

   if(NULL == pszClass || '\0' == *pszClass)
   {
      return E_FAIL;
   }

   // Orginally 500ms in SmartPhone 2003 App Wizard generated code
   // A lower value will result in a more responsive start-up of 
   // the existing instance or termination of this instance.
   const DWORD dRetryInterval = 100; 

   // Orginally 5 in SmartPhone 2003 App Wizard generated code
   // Multiplied by 5, since wait time was divided by 5.
   const int iMaxRetries = 25;

   for(int i = 0; i < iMaxRetries; ++i)
   {
      // Don't need ownership of the mutex
      HANDLE hMutex = CreateMutex(NULL, FALSE, pszClass);

      DWORD dw = GetLastError();

      if(NULL == hMutex)
      {
         HRESULT hr;

         switch(dw)
         {
         case ERROR_INVALID_HANDLE:
            // A non-mutext object with this name already exists.
            hr = E_INVALIDARG;
            break;
         default:
            // This should never happen...
            hr = E_FAIL;
         }

         return hr;
      }

      // If the mutex already exists, then there should be another instance running
      if(ERROR_ALREADY_EXISTS == dw)
      {
         // Just needed the error result, in this case, so close the handle.
         CloseHandle(hMutex);

         // Try to find the other instance, don't need to close HWND's.
         // Don't check title in case it is changed by app after init.
         HWND hwnd = FindWindow(pszClass, NULL);

         if(NULL == hwnd)
         {
            // It's possible that the other istance is in the process of starting up or shutting down.
            // So wait a bit and try again.
            Sleep(dRetryInterval);
            continue;
         }
         else
         {
            // Set the previous instance as the foreground window

            // The "| 0x1" in the code below activates the correct owned window 
            // of the previous instance's main window according to the SmartPhone 2003
            // wizard generated code.
            if(0 != SetForegroundWindow(reinterpret_cast<HWND>(reinterpret_cast<ULONG>(hwnd) | 0x1)))
            {
               // S_FALSE indicates that another instance was activated, so this instance should terminate.
               return S_FALSE;
            }
         }
      }
      else
      {
         // This is the first istance, so return S_OK.
         // Don't close the mutext handle here.
         // Do it on app shutdown instead.
         return S_OK;
      }
   }
#endif

   // The mutex was created by another instance, but it's window couldn't be brought
   // to the foreground, so ssume  it's not a invalid instance (not this app, hung, etc.)
   // and let this one start.
   return S_OK;
}
