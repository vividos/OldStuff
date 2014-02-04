//
// MrCrash - crash info reporting tool
// Copyright (C) 2005-2014 Michael Fink
//
/// \file MrCrash.cpp Main function
//

// includes
#include "stdafx.h"
#include "resource.h"
#include "AppOptions.hpp"
#include "SelectDebugTargetDlg.hpp"
#include "SaveResultsDlg.hpp"

/// Mr. Crash main function
void MrCrash(LPCTSTR lpstrCmdLine)
{
   _tprintf(_T("MrCrash - Reports your application's errors\n\n"));

   AppOptions opt(lpstrCmdLine);
   Debugger d;

   switch (opt.StartMode())
   {
   case smStartProcess:
      d.Start(opt.StartApp(), opt.CmdLine(), false);
      break;

   case smAttachProcess:
      d.Attach(opt.AttachProcessId(), false);
      break;

   case smInteractive:
      {
         SelectDebugTargetDlg dlg;
         if (IDOK == dlg.DoModal())
         {
            switch (dlg.GetDebugType())
            {
            case dtStartApplication:
               d.Start(dlg.GetApplication(), dlg.GetCommandLine(), dlg.GetAlwaysCollectReport());
               break;
            case dtAttachProcess:
               d.Attach(dlg.GetProcessId(), dlg.GetAlwaysCollectReport());
               break;
            }
         }
      }
      break;
   };

   if (!d.GetResults().empty())
   {
      SaveResultsDlg dlg(d.GetResults());
      dlg.DoModal();
   }
}

/// app module
CAppModule _Module;

/// main function
int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR /*lpstrCmdLine*/, int /*nCmdShow*/)
{
   HRESULT hRes = ::CoInitialize(NULL);
   ATLVERIFY(SUCCEEDED(hRes));

   // this resolves ATL window thunking problem when Microsoft Layer for Unicode (MSLU) is used
   ::DefWindowProc(NULL, 0, 0, 0L);

   AtlInitCommonControls(ICC_BAR_CLASSES);

   hRes = _Module.Init(NULL, hInstance);
   ATLVERIFY(SUCCEEDED(hRes));

   MrCrash(GetCommandLine());

   _Module.Term();
   ::CoUninitialize();

   return 0;
}
