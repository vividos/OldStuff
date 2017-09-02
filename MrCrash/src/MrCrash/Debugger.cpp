//
// MrCrash - crash info reporting tool
// Copyright (C) 2005-2014 Michael Fink
//
/// \file Debugger.cpp Debugger class
//

// includes
#include "StdAfx.h"
#include "Debugger.hpp"
#include <ulib/win32/PrivilegeManager.hpp>
#include <ulib/debug/DebugLoop.hpp>
#include <ulib/debug/ToolHelp.hpp>
#include <ulib/debug/CallStack.hpp>
#include <ulib/debug/Toolhelp.hpp>
#include <ulib/win32/VersionInfoResource.hpp>
#include <ulib/win32/ErrorMessage.hpp>
#include <ulib/win32/SystemTime.hpp>
#include <ulib/win32/ProcessHandle.hpp>
#include <ulib/DateTime.hpp>
#include <ulib/stream/TextFileStream.hpp>
#include "ExceptionInfo.hpp"
#include "DebugInfoXmlWriter.hpp"

CString GetModuleFileVersion(LPCTSTR pszModuleName)
{
   if (INVALID_FILE_ATTRIBUTES != ::GetFileAttributes(pszModuleName))
   {
      Win32::VersionInfoResource info(pszModuleName);
      if (info.IsAvail())
      {
         Win32::FixedFileInfo* pFixedInfo = info.GetFixedFileInfo();
         if (pFixedInfo != NULL)
            return pFixedInfo->GetFileVersion();
      }
   }
   return _T("");
}

Debugger::Debugger()
{
   HANDLE hToken;
   if (TRUE == ::OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
   {
      // set debug privilege
      Win32::PrivilegeManager privilegeManager(hToken);
      if (!privilegeManager.Get(SE_DEBUG_NAME))
      {
         if (!privilegeManager.Set(SE_DEBUG_NAME, true))
            ATLTRACE(_T("DEBUG: couldn't get debug privilege, trying without it...\n"));
      }

      if (hToken != INVALID_HANDLE_VALUE)
         CloseHandle(hToken);
   }
}

Debugger::~Debugger()
{
   HANDLE hToken = INVALID_HANDLE_VALUE;
   if (TRUE == ::OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
   {
      // remove debug privilege
      Win32::PrivilegeManager privilegeManager(hToken);
      privilegeManager.Set(SE_DEBUG_NAME, false);

      if (hToken != INVALID_HANDLE_VALUE)
         CloseHandle(hToken);
   }

   // delete all result files
   for (size_t i=0, iMax=m_vecResults.size(); i<iMax; i++)
   {
      DeleteFile(m_vecResults[i].m_cszFilename);
   }
}

void Debugger::Attach(DWORD dwProcessId, bool bAlwaysWriteReport)
{
   ATLASSERT(GetCurrentProcessId() != dwProcessId); // cannot debug own process

   SystemTime stStartTime;
   GetLocalTime(&stStartTime);

   BOOL bRet = ::DebugActiveProcess(dwProcessId);

   if (bRet == TRUE)
   {
      // TODO needed?
//      InsertDefaultProcessInfo(dwProcessId, stStartTime);

      // run debug loop
      Debug::DebugLoop debugLoop;
      debugLoop.AddObserver(this);
      debugLoop.Run();
      debugLoop.RemoveObserver(this);
   }
   else
   {
      ATLTRACE(_T("Attach failed: %s"), Win32::ErrorMessage().ToString().GetString());
   }

   if (bAlwaysWriteReport && m_vecResults.empty())
      WriteCrashReport();
}

void Debugger::Start(const CString& cszApplication, const CString& cszCommandLine, bool bAlwaysWriteReport)
{
   // start process
   PROCESS_INFORMATION processInfo;
   SecureZeroMemory(&processInfo, sizeof(processInfo));

   STARTUPINFO startupInfo;
   SecureZeroMemory(&startupInfo, sizeof(startupInfo));
   startupInfo.cb = sizeof(startupInfo);

   BOOL bRet = CreateProcess(
      cszApplication,
      const_cast<LPTSTR>(static_cast<LPCTSTR>(cszCommandLine)),
      NULL, NULL,
      FALSE,
      DEBUG_PROCESS, // <<<
      NULL,
      NULL,
      &startupInfo,
      &processInfo);

   if (bRet != TRUE)
      return;

   CloseHandle(processInfo.hProcess);
   CloseHandle(processInfo.hThread);

   // run debug loop
   Debug::DebugLoop debugLoop;
   debugLoop.AddObserver(this);
   debugLoop.Run();
   debugLoop.RemoveObserver(this);

   if (bAlwaysWriteReport && m_vecResults.empty())
      WriteCrashReport();
}

void Debugger::OnCreateProcess(DWORD dwProcessId, DWORD dwThreadId, LPVOID pImageBase, LPVOID pStartProc, LPCTSTR pszImageName)
{
   SystemTime stStartTime;
   GetLocalTime(&stStartTime);

   // add process info
   Debug::ProcessInfo pi(dwProcessId);

   pi.ImageName(pszImageName);
   pi.ImageAddress(pImageBase);
   pi.StartTime(stStartTime);
   pi.ImageVersion(GetModuleFileVersion(pszImageName)); // TODO move to ...

   // add thread info
   Debug::ThreadInfo ti(dwThreadId);
   ti.StartTime(stStartTime);
   ti.StartProc(pStartProc);

   pi.AddThread(ti);

   m_debuggeeInfo.AddProcess(pi);

   // retrieve more infos for this process id at the next opportunity
   // this is done since at this point the process isn't known to the kernel/symbol info yet
   m_setRemainingProcessInfo.insert(dwProcessId);

   // TODO get parent process id, and insert into multimap
   {
      Debug::Toolhelp::Snapshot ts(TH32CS_SNAPPROCESS | TH32CS_SNAPNOHEAPS);

      Debug::Toolhelp::ProcessList processList(ts);

      for (size_t i = 0; i< processList.GetCount(); i++)
      {
         const PROCESSENTRY32& pe = processList.GetEntry(i);
         if (pe.th32ProcessID == dwProcessId)
         {
            // store parent process id and process id in child process map
            DWORD dwParentProcessId = pe.th32ParentProcessID;
            if (dwParentProcessId == ::GetCurrentProcessId())
               dwParentProcessId = 0;

//            ATLTRACE(_T("parent: %08x, pid: %08x\n"), dwParentProcessId, dwProcessId);

            m_mmapChildProcesses.insert(std::make_pair(dwParentProcessId, dwProcessId));

//            AddChildProcess(pe.th32ParentProcessID, dwProcessId);
            break;
         }
      }
   }

   CString cszText;
   cszText.Format(_T("DEBUG: CreateProcess, pid=%08x tid=%08x imagebase=%p name=%s startproc=%p"),
      dwProcessId, dwThreadId, pImageBase, pszImageName, pStartProc);
   RecordEvent(cszText);
}

void Debugger::InsertDefaultProcessInfo(DWORD dwProcessId, const SystemTime& stStartTime)
{
   // create new process info
   Debug::ProcessInfo pi(dwProcessId);
   pi.StartTime(stStartTime);

   Debug::Toolhelp::Snapshot ts(TH32CS_SNAPPROCESS | TH32CS_SNAPTHREAD | TH32CS_SNAPMODULE | TH32CS_SNAPNOHEAPS);

   // figure out further process infos
   {
      Debug::Toolhelp::ProcessList processList(ts);

      for (size_t i = 0; i< processList.GetCount(); i++)
      {
         const PROCESSENTRY32& pe = processList.GetEntry(i);
         if (pe.th32ProcessID == dwProcessId)
         {
            pi.ImageName(pe.szExeFile);
//            pi.SetImageAddress(pe.); // TODO

            // figure out running threads
            Debug::Toolhelp::ThreadList threadList(ts, pe);

            for (size_t j = 0; j< threadList.GetCount(); j++)
            {
               const THREADENTRY32& te = threadList.GetEntry(j);

               Debug::ThreadInfo ti(te.th32ThreadID);
               ti.StartTime(stStartTime);
               ti.StartProc(NULL); // TODO

               pi.AddThread(ti);
            }

            // figure out loaded libraries
            Debug::Toolhelp::ModuleList moduleList(ts, pe);

            for (size_t k = 0; k< moduleList.GetCount(); k++)
            {
               const MODULEENTRY32& me = moduleList.GetEntry(k);

               Debug::ModuleInfo li(me.modBaseAddr, me.szExePath);
               li.LoadTime(stStartTime);
               li.Size(me.modBaseSize);

               li.FileVersion(GetModuleFileVersion(me.szExePath));
            }

            break; // found process id
         }
      }
   }
}

void Debugger::OnCreateThread(DWORD dwProcessId, DWORD dwThreadId, LPVOID pStartProc)
{
   SystemTime stStartTime;
   GetLocalTime(&stStartTime);

   // check process info
   if (!m_debuggeeInfo.IsProcessInfoAvail(dwProcessId))
   {
      RecordEvent(_T("DEBUG: no process info available for pid!"));

      InsertDefaultProcessInfo(dwProcessId, stStartTime);
      ATLASSERT(true == m_debuggeeInfo.IsProcessInfoAvail(dwProcessId));
   }

   RetrieveRemainingInfos();

   // add thread info
   Debug::ProcessInfo pi = m_debuggeeInfo.GetProcessInfo(dwProcessId);
   Debug::ThreadInfo ti(dwThreadId);
   // TODO set more thread info properties
   ti.StartProc(pStartProc);
   ti.StartTime(stStartTime);

   pi.AddThread(ti);

   // find out function name
   Debug::Symbol symbol;
   pi.SymbolManager().GetSymFromAddr((DWORD64)pStartProc, symbol);
   CString cszFunctionName = symbol.GetName();

   // output event
   CString cszText;
   cszText.Format(_T("DEBUG: CreateThread, pid=%08x tid=%08x startproc=%p function=[%s]"),
      dwProcessId, dwThreadId, pStartProc, cszFunctionName.GetString());

   RecordEvent(cszText);
}

void Debugger::OnExitThread(DWORD dwProcessId, DWORD dwThreadId, DWORD dwExitCode)
{
   SystemTime stStopTime;
   GetLocalTime(&stStopTime);

   // check process info
   if (!m_debuggeeInfo.IsProcessInfoAvail(dwProcessId))
   {
      RecordEvent(_T("DEBUG: no process info available for pid!"));

      InsertDefaultProcessInfo(dwProcessId, stStopTime);
      ATLASSERT(true == m_debuggeeInfo.IsProcessInfoAvail(dwProcessId));
   }

   RetrieveRemainingInfos();

   Debug::ProcessInfo pi = m_debuggeeInfo.GetProcessInfo(dwProcessId);

   if (!pi.IsThreadInfoAvail(dwThreadId))
   {
      RecordEvent(_T("DEBUG: no thread info available for tid!"));

      // add thread info
      Debug::ThreadInfo ti(dwThreadId);
      ti.StartTime(stStopTime);
      pi.AddThread(ti);
   }

   // set properties
   Debug::ThreadInfo ti = pi.GetThreadInfo(dwThreadId);
   ti.StopTime(stStopTime);
   ti.ExitCode(dwExitCode);

   pi.AbandonThread(dwThreadId);

   CString cszText;
   cszText.Format(_T("DEBUG: ExitThread, pid=%08x tid=%08x exitcode=%08x (%d)"),
      dwProcessId, dwThreadId, dwExitCode, dwExitCode);
   RecordEvent(cszText);
}

bool Debugger::OnExitProcess(DWORD dwProcessId, DWORD dwThreadId, DWORD dwExitCode)
{
   SystemTime stStopTime;
   GetLocalTime(&stStopTime);

   // check process info
   if (!m_debuggeeInfo.IsProcessInfoAvail(dwProcessId))
   {
      RecordEvent(_T("DEBUG: no process info available for pid!"));

      InsertDefaultProcessInfo(dwProcessId, stStopTime);
      ATLASSERT(true == m_debuggeeInfo.IsProcessInfoAvail(dwProcessId));
   }

   RetrieveRemainingInfos();

   Debug::ProcessInfo pi = m_debuggeeInfo.GetProcessInfo(dwProcessId);

   // set properties
   pi.StopTime(stStopTime);
   pi.ExitCode(dwExitCode);

   // abandon all threads and libraries
   pi.AbandonAllThreads(stStopTime);
/*
   {
      const Debug::ProcessInfo::T_mapLoadedModules& mapLoadedModules = pi.GetAllLoadedModules();
      const Debug::ProcessInfo::T_setUnloadedModules& setUnloadedModules = pi.GetAllUnloadedModules();
      size_t s1 = mapLoadedModules.size();
      size_t s2 = setUnloadedModules.size();
      ATLTRACE(_T("loaded modules: %u, unloaded modules: %u\n"), s1, s2);
   }
*/
   pi.UnloadAllModules(stStopTime);
/*
   {
      const Debug::ProcessInfo::T_mapLoadedModules& mapLoadedModules = pi.GetAllLoadedModules();
      const Debug::ProcessInfo::T_setUnloadedModules& setUnloadedModules = pi.GetAllUnloadedModules();
      size_t s1 = mapLoadedModules.size();
      size_t s2 = setUnloadedModules.size();
      ATLTRACE(_T("loaded modules: %u, unloaded modules: %u\n"), s1, s2);
   }
*/
   // remove process info from debuggee
   m_debuggeeInfo.AbandonProcess(dwProcessId);
/*
   {
      const Debug::ProcessInfo::T_mapLoadedModules& mapLoadedModules = pi.GetAllLoadedModules();
      const Debug::ProcessInfo::T_setUnloadedModules& setUnloadedModules = pi.GetAllUnloadedModules();
      size_t s1 = mapLoadedModules.size();
      size_t s2 = setUnloadedModules.size();
      ATLTRACE(_T("loaded modules: %u, unloaded modules: %u\n"), s1, s2);
   }
*/
   CString cszText;
   cszText.Format(_T("DEBUG: ExitProcess, pid=%08x tid=%08x exitcode=%08x (%d)"),
      dwProcessId, dwThreadId, dwExitCode, dwExitCode);
   RecordEvent(cszText);

   // still processes available to debug?
   if (m_debuggeeInfo.GetCurrentProcessCount() > 0)
      return true; // yes, continue
   else
      return false; // no further processes
}

void Debugger::OnLoadDll(DWORD dwProcessId, DWORD dwThreadId, LPVOID pBaseAddress, LPCTSTR pszDllName)
{
   SystemTime stLoadTime;
   GetLocalTime(&stLoadTime);

   // check process info
   if (!m_debuggeeInfo.IsProcessInfoAvail(dwProcessId))
   {
      RecordEvent(_T("DEBUG: no process info available for pid!"));

      InsertDefaultProcessInfo(dwProcessId, stLoadTime);
      ATLASSERT(true == m_debuggeeInfo.IsProcessInfoAvail(dwProcessId));
   }

   RetrieveRemainingInfos();

   Debug::ProcessInfo pi = m_debuggeeInfo.GetProcessInfo(dwProcessId);

   // find out dll name when not known
   CString cszDllName;
   if (pszDllName == NULL || *pszDllName == 0 ||
      INVALID_FILE_ATTRIBUTES == ::GetFileAttributes(pszDllName))
   {
      Debug::SymModuleInfo moduleInfo;
      pi.SymbolManager().GetModuleInfo((DWORD64)pBaseAddress, moduleInfo);
      // TODO
   }
   else
      cszDllName = pszDllName;

   // add library info
   Debug::ModuleInfo mi(pBaseAddress, pszDllName);
   {
      Debug::SymModuleInfo moduleInfo;
      pi.SymbolManager().GetModuleInfo((DWORD64)pBaseAddress, moduleInfo);
      DWORD dwSize = moduleInfo.ImageSize;

      Debug::Toolhelp::Snapshot ts(TH32CS_SNAPMODULE32, dwProcessId);

      if ((HANDLE)ts != NULL)
      {
         PROCESSENTRY32 pe = {0};
         pe.th32ProcessID = dwProcessId;
         Debug::Toolhelp::ModuleList moduleList(ts, pe);

         for (size_t i=0,iMax=moduleList.GetCount(); i<iMax; i++)
         {
            const MODULEENTRY32& entry = moduleList.GetEntry(i);
            if (entry.modBaseAddr == pBaseAddress)
            {
               dwSize = entry.modBaseSize;
               break;
            }
         }
      }

      mi.Size(dwSize);
   }

   // TODO figure out further process infos
   {
//      Debug::Toolhelp::ProcessList processList(ts);



   }
   mi.LoadTime(stLoadTime);
   mi.FileVersion(GetModuleFileVersion(pszDllName));

   pi.AddModule(mi);

   m_setRemainingModuleInfo.insert(std::make_pair(dwProcessId, pBaseAddress));

   CString cszText;
   cszText.Format(_T("DEBUG: LoadDll, pid=%08x tid=%08x dllbase=%p name=%s version=%s"),
      dwProcessId, dwThreadId, pBaseAddress, pszDllName, mi.FileVersion().GetString());
   RecordEvent(cszText);
}

void Debugger::OnUnloadDll(DWORD dwProcessId, DWORD dwThreadId, LPVOID pBaseAddress)
{
   SystemTime stUnloadTime;
   GetLocalTime(&stUnloadTime);

   // check process info
   if (!m_debuggeeInfo.IsProcessInfoAvail(dwProcessId))
   {
      RecordEvent(_T("DEBUG: no process info available for pid!"));

      InsertDefaultProcessInfo(dwProcessId, stUnloadTime);
      ATLASSERT(true == m_debuggeeInfo.IsProcessInfoAvail(dwProcessId));
   }

   RetrieveRemainingInfos();

   Debug::ProcessInfo pi = m_debuggeeInfo.GetProcessInfo(dwProcessId);

   // check library info
   if (!pi.IsModuleInfoAvail(pBaseAddress))
   {
      Debug::ModuleInfo mi(pBaseAddress, _T("(unknown)"));
      mi.LoadTime(stUnloadTime);
      mi.Size(0); // TODO

      pi.AddModule(mi);
   }

   Debug::ModuleInfo mi = pi.GetModuleInfo(pBaseAddress);
   mi.UnloadTime(stUnloadTime);

   pi.UnloadModule(pBaseAddress);

   CString cszText;
   cszText.Format(_T("DEBUG: UnloadDll, pid=%08x tid=%08x dllbase=%p name=%s"),
      dwProcessId, dwThreadId, pBaseAddress, mi.ModuleName().GetString(), mi.FileVersion().GetString());
   RecordEvent(cszText);
}

void Debugger::OnOutputDebugString(DWORD dwProcessId, DWORD dwThreadId, LPCTSTR pszText)
{
   RetrieveRemainingInfos();

   CString cszOutputText(pszText);
   cszOutputText.TrimRight(_T("\r\n"));
   cszOutputText.Replace(_T("\n"), _T("\\n"));
   cszOutputText.Replace(_T("\r"), _T("\\r"));

   CString cszText;
   cszText.Format(_T("DEBUG: OutputDebugString, pid=%08x tid=%08x text=[%s]"),
      dwProcessId, dwThreadId, cszOutputText.GetString());
   RecordEvent(cszText);
}

void Debugger::OnException(DWORD dwProcessId, DWORD dwThreadId, EXCEPTION_DEBUG_INFO& debugInfo)
{
   RetrieveRemainingInfos();

   CString cszText;
   cszText.Format(_T("DEBUG: Exception, pid=%08x tid=%08x %s chance, code=%08x address=%08x"),
      dwProcessId, dwThreadId,
      debugInfo.dwFirstChance == 0 ? _T("2nd") : _T("1st"),
      debugInfo.ExceptionRecord.ExceptionCode,
      debugInfo.ExceptionRecord.ExceptionAddress
   );

   // output exception info, only for second-chance exceptions
   if (debugInfo.dwFirstChance == 0)
   {
      Debug::ExceptionInfo exceptionInfo(dwProcessId, dwThreadId, debugInfo);

      const Debug::CallStack& callStack = exceptionInfo.Callstack();

      // save callstack
      if (callStack.GetSize() > 0)
      {
         for (size_t i=0; i<callStack.GetSize(); i++)
         {
            const Debug::FunctionCall& fc = callStack.GetFunctionCall(i);
            ATLTRACE(_T("frame %u: name = %s\n"), i, fc.GetFunctionName().GetString());
            fc;
         }
      }

      WriteCrashReport(exceptionInfo);
      WriteMiniDump(dwProcessId, dwThreadId);
   }

   RecordEvent(cszText);
}

CString Debugger::FormatLocalSystemTime(SYSTEMTIME& stTime)
{
   CString cszDate;
   ::GetDateFormat(LOCALE_USER_DEFAULT, 0, &stTime, _T("'['yyyy'-'MM'-'dd' '"), cszDate.GetBuffer(64), 64);
   cszDate.ReleaseBuffer();

   CString cszTime;
   ::GetTimeFormat(LOCALE_USER_DEFAULT, TIME_FORCE24HOURFORMAT, &stTime, _T("HH':'mm':'ss']'"), cszTime.GetBuffer(64), 64);
   cszTime.ReleaseBuffer();

   return cszDate + cszTime;
}

void Debugger::RetrieveRemainingInfos()
{
   std::set<DWORD>::const_iterator iterProcessInfo = m_setRemainingProcessInfo.begin(),
      stopProcessInfo = m_setRemainingProcessInfo.end();

   for (; iterProcessInfo != stopProcessInfo; iterProcessInfo++)
   {
      DWORD dwProcessId = *iterProcessInfo;
      ATLASSERT(TRUE == m_debuggeeInfo.IsProcessInfoAvail(dwProcessId));

      Debug::ProcessInfo pi = m_debuggeeInfo.GetProcessInfo(dwProcessId);

      Debug::SymModuleInfo moduleInfo;
      pi.SymbolManager().GetModuleInfo((DWORD64)pi.ImageAddress(), moduleInfo);

      _asm nop;


/*
// TODO do this for thread, not process
      // find out entry function name
      Debug::Symbol symbol;
      pi.SymbolManager().GetSymFromAddr((DWORD64)pStartProc, symbol);
      CString cszFunctionName = symbol.GetName();
      pi.
*/
   }

   m_setRemainingProcessInfo.clear();

   {
      std::set<std::pair<DWORD, LPVOID> >::const_iterator iterModuleInfo = m_setRemainingModuleInfo.begin(),
         stopModuleInfo = m_setRemainingModuleInfo.end();

      for (; iterModuleInfo != stopModuleInfo; iterModuleInfo++)
      {
         DWORD dwProcessId = iterModuleInfo->first;
         LPVOID pBaseAddress = iterModuleInfo->second;

         ATLASSERT(TRUE == m_debuggeeInfo.IsProcessInfoAvail(dwProcessId));

         Debug::ProcessInfo pi = m_debuggeeInfo.GetProcessInfo(dwProcessId);

         ATLASSERT(TRUE == pi.IsModuleInfoAvail(pBaseAddress));

         Debug::ModuleInfo mi = pi.GetModuleInfo(pBaseAddress);

         Debug::SymModuleInfo moduleInfo;
         pi.SymbolManager().GetModuleInfo((DWORD64)pBaseAddress, moduleInfo);
         mi.Size(moduleInfo.ImageSize);
      }

      m_setRemainingModuleInfo.clear();
   }
}

void Debugger::RecordEvent(const CString& cszText)
{
#if defined(DEBUG) || defined(_DEBUG)
   OutputDebugString(cszText);
   OutputDebugString(_T("\n"));
#endif

   m_vecDebugEvents.push_back(DebugEvent(cszText));
}

void Debugger::WriteCrashReport(boost::optional<const Debug::ExceptionInfo&> optExceptionInfo)
{
   DWORD dwProcessId = 0; // TODO
   CString cszFilename = GenerateTempFilename(dwProcessId, _T("MrCrash-Report")) + _T(".xml");

   Stream::TextFileStream fs(cszFilename,
      Stream::FileStream::modeCreate,
      Stream::FileStream::accessReadWrite,
      Stream::FileStream::shareRead);

   // write out BOM
   CString cszBOM((WCHAR)Stream::TextFileStream::BOM);
   Stream::ITextStream& its = fs;
   its.Write(cszBOM);

   XML::XmlWriterPtr spWriter = XML::XmlWriterPtr(new XML::XmlWriter(fs));

   spWriter->WriteXmlDeclaration(_T("1.0"), true);

   spWriter->WriteStartElement(_T("report"));
   spWriter->WriteStartElementEnd();

   DebugInfoXmlWriter dixw(spWriter);

   // should write exception report or standard report?
   if (optExceptionInfo.is_initialized())
      dixw.WriteExceptionReport(m_debuggeeInfo, optExceptionInfo.get(), m_mmapChildProcesses, m_vecDebugEvents);
   else
      dixw.WriteStandardReport(m_debuggeeInfo, m_mmapChildProcesses, m_vecDebugEvents);

   spWriter->WriteEndElement(); // element report

   // add to results list
   DebuggerResultEntry entry;

   entry.m_cszTitle.Format(
      optExceptionInfo.is_initialized() ? _T("Crash Report (%s)") : _T("Application Report (%s)"),
      DateTime::Now().Format(_T("%Y-%m-%d %H:%M:%S")).GetString());

   entry.m_cszFilename = cszFilename;
   m_vecResults.push_back(entry);
}

void Debugger::WriteMiniDump(DWORD dwProcessId, DWORD dwThreadId)
{
   CString cszFilename = GenerateTempFilename(dwProcessId, _T("MrCrash-Minidump")) + _T(".mdmp");

   Win32::ProcessHandle ph(dwProcessId, PROCESS_QUERY_INFORMATION | PROCESS_VM_READ);

   HANDLE hFile = ::CreateFile(cszFilename,
      GENERIC_WRITE,
      FILE_SHARE_READ,
      NULL,
      CREATE_ALWAYS,
      FILE_ATTRIBUTE_NORMAL,
      NULL);
   ATLVERIFY(hFile != INVALID_HANDLE_VALUE);

   MINIDUMP_EXCEPTION_INFORMATION exceptionInfo; // TODO fill
   exceptionInfo.ThreadId = dwThreadId;
   // exceptionInfo.ExceptionPointers = ; // TODO
   exceptionInfo.ClientPointers = TRUE; // memory resides in the debugged process

   BOOL fRet = ::MiniDumpWriteDump(ph, dwProcessId, hFile,
      MiniDumpWithFullMemory, NULL, NULL, NULL);
   ATLVERIFY(fRet != FALSE);
   if (fRet == FALSE)
   {
      HRESULT hr = GetLastError();
      ATLTRACE(_T("error writing minidump: 0x%08x\n"), hr);
      hr;
   }

   CloseHandle(hFile);

   // add to results list
   DebuggerResultEntry entry;
   entry.m_cszTitle.Format(_T("Minidump (%s)"),
      DateTime::Now().Format(_T("%Y-%m-%d %H:%M:%S")).GetString());
   entry.m_cszFilename = cszFilename;
   m_vecResults.push_back(entry);
}

CString Debugger::GenerateTempFilename(DWORD dwProcessId, const CString& cszFilenamePart)
{
   CString cszTempPath;
   ::GetTempPath(MAX_PATH, cszTempPath.GetBuffer(MAX_PATH));
   cszTempPath.ReleaseBuffer();

   if (cszTempPath.Right(1) != _T("\\"))
      cszTempPath += _T("\\");

   // filename contains %TEMP%, process id of debugged process, date/time, and running number
   CString cszFilename;

   unsigned int uiUnique = 0;
   do
   {
      cszFilename.Format(_T("%s%s-%08x-%s-%04u"),
         cszTempPath.GetString(),
         cszFilenamePart.GetString(),
         dwProcessId,
         DateTime::Now().Format(_T("%Y%m%d-%H%M%S")).GetString(),
         uiUnique++);
   } while(::GetFileAttributes(cszFilename) != INVALID_FILE_ATTRIBUTES);

   return cszFilename;
}
