#include "stdafx.h"
#include <ulib/debug/ProcessMemory.hpp>

using Debug::ProcessMemory;

bool ProcessMemory::Read(LPCVOID pAddress, LPVOID pBuffer, SIZE_T nLength)
{
   HANDLE hProcess = ::OpenProcess(PROCESS_VM_READ, FALSE, m_dwProcessId);
   if (hProcess == 0)
      return false;

   SIZE_T nReadBytes = 0;
   BOOL bRet = ::ReadProcessMemory(hProcess, pAddress, pBuffer, nLength, &nReadBytes);

   ::CloseHandle(hProcess);

   return bRet == TRUE && nLength == nReadBytes;
}

bool ProcessMemory::Write(LPVOID pAddress, LPCVOID pBuffer, SIZE_T nLength)
{
   HANDLE hProcess = ::OpenProcess(PROCESS_VM_WRITE | PROCESS_VM_OPERATION, FALSE, m_dwProcessId);
   if (hProcess == 0)
      return false;

   SIZE_T nWrittenBytes = 0;
   BOOL bRet = ::WriteProcessMemory(hProcess, pAddress, pBuffer, nLength, &nWrittenBytes);

   ::CloseHandle(hProcess);

   return bRet == TRUE && nLength == nWrittenBytes;
}
