//
// Recorder - a GPS logger app for Windows Mobile
// Copyright (C) 2006-2019 Michael Fink
//
/// \file Recorder.cpp Recorder main function
//
#include "stdafx.h"
#include "resourceppc.h"
#include "RecorderView.h"
#include "AboutDlg.h"
#include "RecorderFrame.h"

CAppModule _Module;

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int nCmdShow)
{
   HRESULT hRes = CRecorderFrame::ActivatePreviousInstance(hInstance, lpstrCmdLine);

   if (FAILED(hRes) || S_FALSE == hRes)
   {
      return hRes;
   }

   hRes = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
   ATLASSERT(SUCCEEDED(hRes));

   AtlInitCommonControls(ICC_DATE_CLASSES);
   SHInitExtraControls();

   hRes = _Module.Init(NULL, hInstance);
   ATLASSERT(SUCCEEDED(hRes));

   int nRet = CRecorderFrame::AppRun(lpstrCmdLine, nCmdShow);

   _Module.Term();
   ::CoUninitialize();

   return nRet;
}
