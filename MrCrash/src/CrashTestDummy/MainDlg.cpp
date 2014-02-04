//
// MrCrash - crash info reporting tool
// Copyright (C) 2005-2014 Michael Fink
//
/// \file MainDlg.cpp Main dialog
//

// includes
#include "stdafx.h"
#include "resource.h"
#include "MainDlg.hpp"

LRESULT MainDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   CenterWindow();

   // set icons
   HICON hIcon = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME),
      IMAGE_ICON, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON), LR_DEFAULTCOLOR);
   SetIcon(hIcon, TRUE);

   HICON hIconSmall = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME),
      IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);
   SetIcon(hIconSmall, FALSE);

   return TRUE;
}

LRESULT MainDlg::OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   EndDialog(wID);
   return 0;
}

void MainDlg::CrashDebugBreak()
{
   DebugBreak();
}

#pragma warning(push)
#pragma warning(disable:4717) // recursive on all control paths, function will cause runtime stack overflow
void MainDlg::CrashStackOverflow()
{
   CrashStackOverflow();
}
#pragma warning(pop)

void MainDlg::CrashAccessViolationRead()
{
   BYTE* ptr = NULL;
   ptr += 0x1234;
   BYTE test = *ptr; //<-- read access violation
   test;
}

void MainDlg::CrashAccessViolationWrite()
{
   BYTE* ptr = NULL;
   ptr += 0x5678;
   *ptr = 0x42; //<-- write access violation
}

void MainDlg::CrashOutputDebugString()
{
   OutputDebugString(_T("Hello Debug-World!"));
}

void MainDlg::CrashLoadFreeLibrary()
{
   HMODULE hLib = LoadLibrary(_T("Rasapi32.dll"));
   Sleep(100);
   FreeLibrary(hLib);
   FreeLibrary(hLib);
}

void MainDlg::CrashCreateProcess()
{
   TCHAR aszCurrentDirectory[MAX_PATH];
   ::GetCurrentDirectory(MAX_PATH, aszCurrentDirectory);

   STARTUPINFO startupInfo;
   ZeroMemory(&startupInfo, sizeof(startupInfo));
   startupInfo.cb = sizeof(startupInfo);

   PROCESS_INFORMATION processInfo;

   CString cszAppName;
   ::GetModuleFileName(NULL, cszAppName.GetBuffer(MAX_PATH), MAX_PATH);
   cszAppName.ReleaseBuffer();

   ATLVERIFY(TRUE == CreateProcess(
      cszAppName,
      NULL,
      NULL,
      NULL,
      FALSE, 0, NULL,
      aszCurrentDirectory,
      &startupInfo, &processInfo));

   CloseHandle(processInfo.hProcess);
   CloseHandle(processInfo.hThread);
}

static DWORD CALLBACK ThreadProc(LPVOID /*lpParameter*/)
{
   Sleep(10);
   return 0x2a;
}

void MainDlg::CrashCreateThread()
{
   HANDLE hThread = ::CreateThread(NULL, 0, ThreadProc, NULL, 0, NULL);
   CloseHandle(hThread);
}

/*
CrashDivByZero()
{
   int i = 0;
   int n = 0;
   int r = i/n;

   return 0;
}
*/
