// includes
#include "stdafx.h"
#include <ulib/debug/ToolHelp.hpp>
#include <ulib/win32/ErrorMessage.hpp>

#ifdef _WIN32_WCE
#  pragma comment(lib, "toolhelp.lib")
#endif

using Debug::Toolhelp::Snapshot;
using Debug::Toolhelp::ProcessList;
using Debug::Toolhelp::ThreadList;
using Debug::Toolhelp::ModuleList;
using Debug::Toolhelp::HeapList;
using Debug::Toolhelp::HeapEntryList;

Snapshot::Snapshot(DWORD dwFlags, DWORD th32ProcessID)
{
   // create shared pointer with custom deletor
   HANDLE hSnapshot = ::CreateToolhelp32Snapshot(dwFlags, th32ProcessID);

   if (hSnapshot == INVALID_HANDLE_VALUE)
   {
      // note: we don't reset m_spSnapshot here, so it keeps the
      // invalid handle value; it is checked later in the functions
      DWORD dwLastError = GetLastError(); dwLastError;
      ATLTRACE(_T("CreateToolhelp32Snapshot error 0x%08x: %s\n"),
         dwLastError, Win32::ErrorMessage(dwLastError).ToString().GetString());
   }
   else
      m_spSnapshot.reset(hSnapshot,
#ifdef _WIN32_WCE
      CloseToolhelp32Snapshot);
#else
      CloseHandle);
#endif
}

ProcessList::ProcessList()
{
   Snapshot ts(TH32CS_SNAPPROCESS | TH32CS_SNAPNOHEAPS, 0);
   RefreshList(ts);
}

void ProcessList::RefreshList(const Snapshot& ts)
{
   ATLASSERT((HANDLE)ts != NULL);

   m_processList.clear();

   PROCESSENTRY32 pe;
   ZeroMemory(&pe, sizeof(pe));
   pe.dwSize = sizeof(pe);

   BOOL bRet = ::Process32First(ts, &pe);
   if (bRet != FALSE)
   do
   {
      // don't add process ID 0 (system process), since in Snapshot it means
      // "current process", not system process
      if (pe.th32ProcessID != 0)
         m_processList.push_back(pe);

   } while(::Process32Next(ts, &pe) != FALSE);
}

ThreadList::ThreadList()
:m_dwProcessID(0)
{
   Snapshot ts(TH32CS_SNAPTHREAD);
   RefreshList(ts);
}

void ThreadList::RefreshList(const Snapshot& ts)
{
   ATLASSERT((HANDLE)ts != NULL);

   m_threadList.clear();

   THREADENTRY32 te;
   ZeroMemory(&te, sizeof(te));
   te.dwSize = sizeof(te);

   BOOL bRet = ::Thread32First(ts, &te);
   if (bRet != FALSE)
   do
   {
      // only collect threads of given process when process id is known
      if (m_dwProcessID == 0 ||
          (m_dwProcessID != 0 && te.th32OwnerProcessID == m_dwProcessID))
         m_threadList.push_back(te);

   } while(::Thread32Next(ts, &te) != FALSE);
}

void ModuleList::RefreshList(const Snapshot& ts)
{
   ATLASSERT((HANDLE)ts != NULL);

   m_moduleList.clear();

   MODULEENTRY32 me;
   ZeroMemory(&me, sizeof(me));
   me.dwSize = sizeof(me);

   BOOL bRet = ::Module32First(ts, &me);
   if (bRet != FALSE)
   do
   {
      m_moduleList.push_back(me);

   } while(::Module32Next(ts, &me) != FALSE);
}

void HeapList::RefreshList(const Snapshot& ts)
{
   ATLASSERT((HANDLE)ts != NULL);

   m_heapList.clear();

   HEAPLIST32 hl;
   ZeroMemory(&hl, sizeof(hl));
   hl.dwSize = sizeof(hl);

   BOOL bRet = ::Heap32ListFirst(ts, &hl);
   if (bRet != FALSE)
   do
   {
      m_heapList.push_back(hl);

   } while(::Heap32ListNext(ts, &hl) != FALSE);
}

void HeapEntryList::RefreshList(const Snapshot& ts)
{
   ATLASSERT((HANDLE)ts != NULL);

   m_heapEntryList.clear();

   HEAPENTRY32 he;
   ZeroMemory(&he, sizeof(he));
   he.dwSize = sizeof(he);

#ifdef _WIN32_WCE
   BOOL bRet = ::Heap32First(ts, &he, m_dwProcessID, m_ulHeapID);
#else
   ts;
   BOOL bRet = ::Heap32First(&he, m_dwProcessID, m_ulHeapID);
#endif
   if (bRet != FALSE)
   do
   {
      m_heapEntryList.push_back(he);

#ifdef _WIN32_WCE
   } while(::Heap32Next(ts, &he) != FALSE);
#else
   } while(::Heap32Next(&he) != FALSE);
#endif
}
