//
// MrCrash - crash info reporting tool
// Copyright (C) 2005-2014 Michael Fink
//
/// \file CrashTestDummy.cpp Main function
//

// includes
#include "stdafx.h"
#include "resource.h"
#include "MainDlg.hpp"

/// app module
CAppModule _Module;

/// main function
int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR /*lpstrCmdLine*/, int /*nCmdShow*/)
{
   // this resolves ATL window thunking problem when Microsoft Layer for Unicode (MSLU) is used
   ::DefWindowProc(NULL, 0, 0, 0L);

   HRESULT hRes = _Module.Init(NULL, hInstance);
   ATLVERIFY(SUCCEEDED(hRes));

   MainDlg dlgMain;
   int nRet = dlgMain.DoModal();

   _Module.Term();

   return nRet;
}
