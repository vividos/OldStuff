//
// MrCrash - crash info reporting tool
// Copyright (C) 2005-2014 Michael Fink
//
/// \file Debugger.hpp Debugger
//
#pragma once

// includes
#include <ulib/debug/DebugLoop.hpp>
#include "DebugeeInfo.hpp"
#include "DebugEvent.hpp"
#include "ExceptionInfo.hpp"
#include <vector>
#include <map>
#include <boost/optional.hpp>

struct DebuggerResultEntry
{
   CString m_cszTitle;
   CString m_cszFilename;
};

/// debugger instance that collects infos about debuggee
class Debugger: protected Debug::IDebugEventHandler
{
public:
   Debugger();
   virtual ~Debugger();

   void Attach(DWORD dwProcessId, bool bAlwaysWriteReport);

   void Start(const CString& cszApplication, const CString& cszCommandLine, bool bAlwaysWriteReport = false);

   /// returns list with results as {text, filename} pair
   const std::vector<DebuggerResultEntry>& GetResults(){ return m_vecResults; }

protected:
   virtual void OnCreateProcess(DWORD dwProcessId, DWORD dwThreadId, LPVOID pImageBase, LPVOID pStartProc, LPCTSTR pszImageName);
   virtual void OnCreateThread(DWORD dwProcessId, DWORD dwThreadId, LPVOID pStartProc);
   virtual void OnExitThread(DWORD dwProcessId, DWORD dwThreadId, DWORD dwExitCode);
   virtual bool OnExitProcess(DWORD dwProcessId, DWORD dwThreadId, DWORD dwExitCode);

   virtual void OnLoadDll(DWORD dwProcessId, DWORD dwThreadId, LPVOID pBaseAddress, LPCTSTR pszDllName);
   virtual void OnUnloadDll(DWORD dwProcessId, DWORD dwThreadId, LPVOID pBaseAddress);
   virtual void OnOutputDebugString(DWORD dwProcessId, DWORD dwThreadId, LPCTSTR pszText);

   virtual void OnException(DWORD dwProcessId, DWORD dwThreadId, EXCEPTION_DEBUG_INFO& debugInfo);

   CString FormatLocalSystemTime(SYSTEMTIME& stTime);
   void RecordEvent(const CString& cszText);

   void WriteCrashReport(boost::optional<const Debug::ExceptionInfo&> optExceptionInfo =
      boost::optional<const Debug::ExceptionInfo&>());

   void WriteMiniDump(DWORD dwProcessId, DWORD dwThreadId);

   CString GenerateTempFilename(DWORD dwProcessId, const CString& cszFilenamePart);

private:
   void InsertDefaultProcessInfo(DWORD dwProcessId, const SystemTime& stStartTime);

   void RetrieveRemainingInfos();

private:
   Debug::DebugeeInfo m_debuggeeInfo;

   std::set<DWORD> m_setRemainingProcessInfo;
   std::set<std::pair<DWORD, LPVOID> > m_setRemainingModuleInfo;

   CString m_cszEventOutput;

   /// multimap with parent process ids to process ids
   std::multimap<DWORD, DWORD> m_mmapChildProcesses;

   std::vector<DebugEvent> m_vecDebugEvents;

   std::vector<DebuggerResultEntry> m_vecResults;
};
