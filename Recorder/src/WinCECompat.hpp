//
// Recorder - a GPS logger app for Windows Mobile
// Copyright (C) 2006-2019 Michael Fink
//
/// \file WinCECompat.hpp Win32 WinCE compat header
//
#pragma once

template <class T, UINT t_shidiFlags = 0, bool t_bModal = true>
class ATL_NO_VTABLE CStdDialogImpl : public ATL::CDialogImpl<T>
{
   BEGIN_MSG_MAP(CStdDialogImpl)
   END_MSG_MAP()
};

#define DECLARE_APP_FRAME_CLASS(x, id, key)

template <class T>
class ATL_NO_VTABLE CAppWindow
{
public:
   HWND m_hWndCECommandBar;

   BEGIN_MSG_MAP(CAppWindow)
   END_MSG_MAP()

   BOOL CreateSimpleCEMenuBar(UINT nToolBarId = 0, DWORD dwFlags = 0, int nBmpId = 0, int cBmpImages = 0)
   {
      return TRUE;
   }

   static HRESULT ActivatePreviousInstance(HINSTANCE hInstance, LPTSTR lpstrCmdLine)
   {
      return S_OK;
   }

   static int AppRun(LPTSTR lpstrCmdLine, int nCmdShow)
   {
      CMessageLoop theLoop;
      _Module.AddMessageLoop(&theLoop);

      T wndMain;

      if (wndMain.CreateEx(NULL, NULL, 0, 0, lpstrCmdLine) == NULL)
      {
         ATLTRACE2(atlTraceUI, 0, _T("Main window creation failed!\n"));
         return 0;
      }

      wndMain.ShowWindow(nCmdShow);

      int nRet = theLoop.Run();

      _Module.RemoveMessageLoop();
      return nRet;
   }
};

class CAppInfo
{
public:
   void Save(bool, LPCTSTR)
   {
   }

   void Restore(bool, LPCTSTR)
   {
   }
};

inline void SystemIdleTimerReset() {}

inline void SHInitExtraControls() {}
