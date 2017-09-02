#include "stdafx.h"
#include <ulib/debug/DebugLoop.hpp>
#include <ulib/debug/ProcessMemory.hpp>
#include <ulib/debug/ToolHelp.hpp>
#include <ulib/win32/ErrorMessage.hpp>
#include <vector>

#ifdef UNICODE
#define DBGHELP_TRANSLATE_TCHAR
#endif
#pragma warning(push)
#pragma warning(disable: 4091) // 'typedef ': ignored on left of '' when no variable is declared
#include <dbghelp.h>
#pragma warning(pop)

using Debug::DebugLoop;
using Debug::IDebugEventHandler;

void DebugLoop::Run()
{
   DWORD dwContinueStatus = DBG_CONTINUE;    // exception continuation

   for (bool bLoop = true; bLoop;)
   {
      // Wait for a debugging event to occur. The second parameter indicates
      // that the function does not return until a debugging event occurs.
      ::WaitForDebugEvent(&m_debugEvent, INFINITE);

      // Process the debugging event code.
      switch (m_debugEvent.dwDebugEventCode)
      {
      case EXCEPTION_DEBUG_EVENT:
         // Process the exception code. When handling
         // exceptions, remember to set the continuation
         // status parameter (dwContinueStatus). This value
         // is used by the ContinueDebugEventent function.
         OnException(dwContinueStatus);
         break;

      case CREATE_THREAD_DEBUG_EVENT:
         // As needed, examine or change the thread's registers
         // with the GetThreadContext and SetThreadContext functions;
         // and suspend and resume thread execution with the
         // SuspendThread and ResumeThread functions.
         OnCreateThread();
         break;

      case CREATE_PROCESS_DEBUG_EVENT:
         // As needed, examine or change the registers of the
         // process's initial thread with the GetThreadContext and
         // SetThreadContext functions; read from and write to the
         // process's virtual memory with the ReadProcessMemory and
         // WriteProcessMemory functions; and suspend and resume
         // thread execution with the SuspendThread and ResumeThread
         // functions. Be sure to close the handle to the process image
         // file with CloseHandle.
         OnCreateProcess();
         break;

      case EXIT_THREAD_DEBUG_EVENT:
         // Display the thread's exit code.
         OnExitThread();
         break;

      case EXIT_PROCESS_DEBUG_EVENT:
         // Display the process's exit code.
         if (!OnExitProcess())
            bLoop = false;
         break;

      case LOAD_DLL_DEBUG_EVENT:
         // Read the debugging information included in the newly
         // loaded DLL. Be sure to close the handle to the loaded DLL
         // with CloseHandle.
         OnLoadDll();
         break;

      case UNLOAD_DLL_DEBUG_EVENT:
         // Display a message that the DLL has been unloaded.
         OnUnloadDll();
         break;

      case OUTPUT_DEBUG_STRING_EVENT:
         // Display the output debugging string.
         OnOutputDebugString();
         break;
      }

      // Resume executing the thread that reported the debugging event.
      ::ContinueDebugEvent(m_debugEvent.dwProcessId, m_debugEvent.dwThreadId, dwContinueStatus);
   }
}

void DebugLoop::OnException(DWORD& dwContinueStatus)
{
   // don't handle second-chance exceptions
   if (m_debugEvent.u.Exception.dwFirstChance == 0)
      dwContinueStatus = DBG_EXCEPTION_NOT_HANDLED;

   switch (m_debugEvent.u.Exception.ExceptionRecord.ExceptionCode)
   {
   case EXCEPTION_ACCESS_VIOLATION:
      // First chance: Pass this on to the system.
      // Last chance: Display an appropriate error.
      dwContinueStatus = DBG_EXCEPTION_NOT_HANDLED;
      break;

   case EXCEPTION_DATATYPE_MISALIGNMENT:
      // First chance: Pass this on to the system.
      // Last chance: Display an appropriate error.
      dwContinueStatus = DBG_EXCEPTION_NOT_HANDLED;
      break;

   case EXCEPTION_BREAKPOINT:
      // First chance: Display the current
      // instruction and register values.
      break;

   case EXCEPTION_SINGLE_STEP:
      // First chance: Update the display of the
      // current instruction and register values.
      break;

   case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
      break;

   case EXCEPTION_STACK_OVERFLOW:
      dwContinueStatus = DBG_EXCEPTION_NOT_HANDLED;
      break;

   case DBG_CONTROL_C:
      // First chance: Pass this on to the system.
      // Last chance: Display an appropriate error.
      break;

   default:
      // Handle other exceptions.
      break;
   }

   // send event to all handler
   std::set<IDebugEventHandler*>::iterator iter = m_setEventHandler.begin(), stop = m_setEventHandler.end();
   for(; iter != stop; iter++)
      (*iter)->OnException(m_debugEvent.dwProcessId, m_debugEvent.dwThreadId, m_debugEvent.u.Exception);
}

void DebugLoop::OnCreateThread()
{
   std::set<IDebugEventHandler*>::iterator iter = m_setEventHandler.begin(), stop = m_setEventHandler.end();
   for(; iter != stop; iter++)
      (*iter)->OnCreateThread(m_debugEvent.dwProcessId, m_debugEvent.dwThreadId,
         m_debugEvent.u.CreateThread.lpStartAddress);
}

void DebugLoop::OnCreateProcess()
{
   BOOL bRet = SymInitialize(m_debugEvent.u.CreateProcessInfo.hProcess, NULL, FALSE);
   if (bRet == FALSE)
   {
      DWORD dwLastError = GetLastError(); dwLastError;
      ATLTRACE(_T("SymInitialize error 0x%08x: %s\n"),
         dwLastError, Win32::ErrorMessage(dwLastError).ToString().GetString());
   }

   CString cszImageName;
   DWORD dwImageSize = 0;
   if (m_debugEvent.u.CreateProcessInfo.lpImageName != NULL)
   {
      // extract name
      // TODO implement
   }
   else
   {
      if (m_debugEvent.u.CreateProcessInfo.hProcess != NULL)
      {
         // try to find out process name by function QueryFullProcessImageName(), available from Windows Vista
         HMODULE hKernel32 = ::GetModuleHandle(_T("kernel32.dll"));
         if (hKernel32 != NULL)
         {
            typedef BOOL (WINAPI *T_fnQueryFullProcessImageName)(HANDLE, DWORD, LPWSTR, PDWORD);

            T_fnQueryFullProcessImageName fnQueryFullProcessImageName =
               reinterpret_cast<T_fnQueryFullProcessImageName>(GetProcAddress(hKernel32, "QueryFullProcessImageNameW"));

            if (fnQueryFullProcessImageName != NULL)
            {
               //HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, m_debugEvent.dwProcessId);
               HANDLE hProcess = m_debugEvent.u.CreateProcessInfo.hProcess;
               DWORD dwSize = MAX_PATH;
               fnQueryFullProcessImageName(hProcess, 0, cszImageName.GetBuffer(dwSize), &dwSize);
               cszImageName.ReleaseBuffer();
            }
         }

         HMODULE hModPsapi = ::LoadLibrary(_T("psapi.dll"));
         if (hModPsapi != NULL)
         {
/*
            typedef DWORD (WINAPI *T_fnGetModuleFileNameEx)(HANDLE, HMODULE, LPWSTR, DWORD);

            T_fnGetModuleFileNameEx fnGetModuleFileNameEx =
               reinterpret_cast<T_fnGetModuleFileNameEx>(GetProcAddress(hModPsapi, "GetModuleFileNameExW"));

            if (fnGetModuleFileNameEx != NULL)
            {
               HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, m_debugEvent.dwProcessId);
               //HANDLE hProcess = m_debugEvent.u.CreateProcessInfo.hProcess;
               DWORD dw = fnGetModuleFileNameEx(hProcess, NULL, cszImageName.GetBuffer(MAX_PATH), MAX_PATH);
               cszImageName.ReleaseBuffer();

               if (dw == 0)
               {
                  ATLTRACE(_T("GetModuleFileNameEx error: %08x\n"), GetLastError());
               }

               CloseHandle(hProcess);
            }
*/
/*
         // try to find out process name by function GetProcessImageFileName(), available from Windows XP

            typedef DWORD (WINAPI *T_fnGetProcessImageFileName)(HANDLE, LPWSTR, DWORD);

            T_fnGetProcessImageFileName fnGetProcessImageFileName =
               reinterpret_cast<T_fnGetProcessImageFileName>(GetProcAddress(hModPsapi, "GetProcessImageFileNameW"));

            if (fnGetProcessImageFileName != NULL)
            {
               HANDLE hProcess = m_debugEvent.u.CreateProcessInfo.hProcess;
               fnGetProcessImageFileName(hProcess, cszImageName.GetBuffer(MAX_PATH), MAX_PATH);
               cszImageName.ReleaseBuffer();
            }
*/

            FreeLibrary(hModPsapi);
         }
      }

      if (cszImageName.IsEmpty())
      {
         // find out process name per toolhelp functions
         Debug::Toolhelp::Snapshot ths(TH32CS_SNAPPROCESS | TH32CS_SNAPNOHEAPS);
         Debug::Toolhelp::ProcessList processList(ths);

         for (size_t i = 0; i< processList.GetCount(); i++)
         {
            const PROCESSENTRY32& entry = processList.GetEntry(i);
            if (m_debugEvent.dwProcessId == entry.th32ProcessID)
            {
               cszImageName = entry.szExeFile;
               dwImageSize = entry.dwSize;
               break;
            }
         }
      }
   }

   // initialize symbols
   USES_CONVERSION;
   DWORD64 dwRet = SymLoadModule64(m_debugEvent.u.CreateProcessInfo.hProcess,
      m_debugEvent.u.CreateProcessInfo.hFile,
      T2CA(cszImageName),
      NULL,
      reinterpret_cast<DWORD64>(m_debugEvent.u.CreateProcessInfo.lpBaseOfImage),
      dwImageSize);

   if (dwRet == 0)
   {
      DWORD dwLastError = GetLastError();
      if (ERROR_SUCCESS != dwLastError)
         ATLTRACE(_T("SymLoadModule64 error 0x%08x: %s\n"),
            dwLastError, Win32::ErrorMessage(dwLastError).ToString().GetString());
   }

   std::set<IDebugEventHandler*>::iterator iter = m_setEventHandler.begin(), stop = m_setEventHandler.end();
   for(; iter != stop; iter++)
      (*iter)->OnCreateProcess(m_debugEvent.dwProcessId, m_debugEvent.dwThreadId,
         m_debugEvent.u.CreateProcessInfo.lpBaseOfImage,
         m_debugEvent.u.CreateProcessInfo.lpStartAddress,
         cszImageName);
}

void DebugLoop::OnExitThread()
{
   std::set<IDebugEventHandler*>::iterator iter = m_setEventHandler.begin(), stop = m_setEventHandler.end();
   for(; iter != stop; iter++)
      (*iter)->OnExitThread(m_debugEvent.dwProcessId, m_debugEvent.dwThreadId,
         m_debugEvent.u.ExitThread.dwExitCode);
}

bool DebugLoop::OnExitProcess()
{
   bool bRet = true;
   std::set<IDebugEventHandler*>::iterator iter = m_setEventHandler.begin(), stop = m_setEventHandler.end();
   for(; iter != stop; iter++)
   {
      bRet &= (*iter)->OnExitProcess(m_debugEvent.dwProcessId, m_debugEvent.dwThreadId,
         m_debugEvent.u.ExitProcess.dwExitCode);
   }

   return bRet;
}

void DebugLoop::OnLoadDll()
{
   LPVOID pBaseAddress = m_debugEvent.u.LoadDll.lpBaseOfDll;

   ProcessMemory processMem(m_debugEvent.dwProcessId);

   CString cszDllName;
   {
      LPVOID pImageName = NULL;
      bool bRet = processMem.Read(m_debugEvent.u.LoadDll.lpImageName, &pImageName, sizeof(pImageName));

      if (bRet && pImageName != NULL)
      {
         BYTE abBuffer[512];
         abBuffer[510] = 0;
         abBuffer[511] = 0;

         DWORD nSize = sizeof(abBuffer)-2;

         // round down to next 64k boundary
         DWORD nLeftInSegment = 0xFFFF-static_cast<DWORD>((reinterpret_cast<DWORD_PTR>(pImageName) & 0xFFFF));
         if (nLeftInSegment < nSize)
            nSize = nLeftInSegment;

         bRet = processMem.Read(pImageName, abBuffer, nSize);
         if (bRet)
         {
            USES_CONVERSION;
            cszDllName = (m_debugEvent.u.LoadDll.fUnicode == 1 ?
               W2CT(reinterpret_cast<LPCWSTR>(abBuffer)) :
               A2CT(reinterpret_cast<LPCSTR>(abBuffer)) );
         }
      }
   }

   // initialize symbols
   USES_CONVERSION;
   HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, m_debugEvent.dwProcessId);
   DWORD64 dwRet = SymLoadModule64(hProcess,
      m_debugEvent.u.LoadDll.hFile,
      T2CA(cszDllName),
      NULL,
      reinterpret_cast<DWORD64>(m_debugEvent.u.LoadDll.lpBaseOfDll),
      0);

   if (dwRet == 0)
   {
      DWORD dwLastError = GetLastError();
      if (ERROR_SUCCESS != dwLastError)
         ATLTRACE(_T("SymLoadModule64 error 0x%08x: %s\n"),
            dwLastError, Win32::ErrorMessage(dwLastError).ToString().GetString());
   }
   CloseHandle(hProcess);

   std::set<IDebugEventHandler*>::iterator iter = m_setEventHandler.begin(), stop = m_setEventHandler.end();
   for(; iter != stop; iter++)
      (*iter)->OnLoadDll(m_debugEvent.dwProcessId, m_debugEvent.dwThreadId,
         pBaseAddress, cszDllName);
}

void DebugLoop::OnUnloadDll()
{
   LPVOID pBaseAddress = m_debugEvent.u.UnloadDll.lpBaseOfDll;

   std::set<IDebugEventHandler*>::iterator iter = m_setEventHandler.begin(), stop = m_setEventHandler.end();
   for(; iter != stop; iter++)
      (*iter)->OnUnloadDll(m_debugEvent.dwProcessId, m_debugEvent.dwThreadId,
         pBaseAddress);
}

void DebugLoop::OnOutputDebugString()
{
   LPVOID pAddr = m_debugEvent.u.DebugString.lpDebugStringData;
   SIZE_T nSize = m_debugEvent.u.DebugString.nDebugStringLength;

   std::vector<BYTE> vecBuffer(nSize+2, 0);

   ProcessMemory processMem(m_debugEvent.dwProcessId);
   processMem.Read(pAddr, &vecBuffer[0], nSize);

   USES_CONVERSION;
   LPCTSTR pszText = m_debugEvent.u.DebugString.fUnicode == 1 ?
      W2CT(reinterpret_cast<LPCWSTR>(&vecBuffer[0])) :
      A2CT(reinterpret_cast<LPCSTR>(&vecBuffer[0]));

   std::set<IDebugEventHandler*>::iterator iter = m_setEventHandler.begin(), stop = m_setEventHandler.end();
   for(; iter != stop; iter++)
      (*iter)->OnOutputDebugString(m_debugEvent.dwProcessId, m_debugEvent.dwThreadId, pszText);
}
