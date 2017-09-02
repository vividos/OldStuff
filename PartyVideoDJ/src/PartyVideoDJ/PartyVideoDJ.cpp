//
// PartyVideoDJ - YouTube crossfading app
// Copyright (C) 2008-2017 Michael Fink
//
/// \file PartyVideoDJ.cpp Main function
//
#include "stdafx.h"
#include "resource.h"
#include "MainFrame.hpp"

CAppModule _Module;

int Run(LPTSTR /*lpstrCmdLine*/ = NULL, int nCmdShow = SW_SHOWDEFAULT)
{
   CMessageLoop theLoop;
   _Module.AddMessageLoop(&theLoop);

   MainFrame wndMain;

   if (wndMain.CreateEx() == NULL)
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
   // remove current directory from search path of LoadLibrary(); see also
   // Microsoft Security Advisory (2269637):
   // http://www.microsoft.com/technet/security/advisory/2269637.mspx
//   ATLVERIFY(TRUE == ::SetDllDirectory(_T("")));

   HRESULT hRes = ::CoInitialize(NULL);
   // If you are running on NT 4.0 or higher you can use the following call instead to
   // make the EXE free threaded. This means that calls come in on a random RPC thread.
   // HRESULT hRes = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
   ATLASSERT(SUCCEEDED(hRes));

   // this resolves ATL window thunking problem when Microsoft Layer for Unicode (MSLU) is used
   ::DefWindowProc(NULL, 0, 0, 0L);

   AtlInitCommonControls(ICC_BAR_CLASSES); // add flags to support other controls

   hRes = _Module.Init(NULL, hInstance, &LIBID_ATLLib);
   ATLASSERT(SUCCEEDED(hRes));

   AtlAxWinInit();

   int nRet = Run(lpstrCmdLine, nCmdShow);

   _Module.Term();
   ::CoUninitialize();

   return nRet;
}
