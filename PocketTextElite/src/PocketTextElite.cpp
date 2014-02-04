//
// PocketTextElite - port of Elite[TM] trading system
// Copyright (C) 2008-2009 Michael Fink
//
/// \file PocketTextElite.cpp Main function
//

// includes
#include "stdafx.h"
#include "resource.h"
#include "MainFrame.hpp"

CAppModule _Module;

int Run(LPTSTR /*lpstrCmdLine*/ = NULL, int nCmdShow = SW_SHOWNORMAL)
{
   CMessageLoop theLoop;
   _Module.AddMessageLoop(&theLoop);

   MainFrame wndMain;

   if(wndMain.CreateEx() == NULL)
   {
      ATLTRACE(_T("Main window creation failed!\n"));
      return 0;
   }

   wndMain.ShowWindow(nCmdShow);

   int nRet = theLoop.Run();

   _Module.RemoveMessageLoop();
   return nRet;
}

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int nCmdShow)
{
   HRESULT hRes;

#ifdef UNDER_CE
   hRes = CMainFrame::ActivatePreviousInstance(hInstance);
   if(FAILED(hRes) || S_FALSE == hRes)
   {
      return hRes;
   }

   /*
   Calling AtlInitCommonControls is not necessary to utilize picture,
   static text, edit box, group box, button, check box, radio button, 
   combo box, list box, or the horizontal and vertical scroll bars.

   Calling AtlInitCommonControls with 0 is required to utilize the spin, 
   progress, slider, list, tree, and tab controls.

   Adding the ICC_DATE_CLASSES flag is required to initialize the 
   date time picker and month calendar controls.

   Add additional flags to support additoinal controls not mentioned above.
   */
   AtlInitCommonControls(ICC_DATE_CLASSES);
#else
   // this resolves ATL window thunking problem when Microsoft Layer for Unicode (MSLU) is used
   ::DefWindowProc(NULL, 0, 0, 0L);

   AtlInitCommonControls(ICC_BAR_CLASSES);   // add flags to support other controls
#endif

   hRes = _Module.Init(NULL, hInstance);
   ATLASSERT(SUCCEEDED(hRes));

   int nRet = Run(lpstrCmdLine, nCmdShow);

   _Module.Term();

   return nRet;
}
