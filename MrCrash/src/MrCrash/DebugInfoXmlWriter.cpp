//
// MrCrash - crash info reporting tool
// Copyright (C) 2005-2014 Michael Fink
//
/// \file DebugInfoXmlWriter.cpp Xml writer for debug infos
//

// includes
#include "stdafx.h"
#include "DebugInfoXmlWriter.hpp"
#include <ulib/DateTime.hpp>
#include "DebugeeInfo.hpp"
#include "ExceptionInfo.hpp"
#include "ProcessInfo.hpp"
#include "ThreadInfo.hpp"
#include "ModuleInfo.hpp"
#include "DebugEvent.hpp"

void DebugInfoXmlWriter::WriteDebugEventList(const std::vector<DebugEvent>& vecDebugEvents)
{
   m_spWriter->WriteStartElement(_T("debug-event-list"));
   CString cszText; cszText.Format(_T("%u"), vecDebugEvents.size());
   m_spWriter->WriteAttributeString(_T("event-count"), cszText);
   m_spWriter->WriteStartElementEnd();

   for (size_t i=0,iMax=vecDebugEvents.size(); i<iMax; i++)
      WriteDebugEvent(i, vecDebugEvents[i]);

   m_spWriter->WriteEndElement();
}

void DebugInfoXmlWriter::WriteDebugEvent(size_t /*iIndex*/, const DebugEvent& debugEvent)
{
   m_spWriter->WriteStartElement(_T("debug-event"));
   WriteTimeAttribute(_T("time"), debugEvent.Time());
   m_spWriter->WriteStartElementEnd();

   m_spWriter->WriteCData(debugEvent.Text());

   m_spWriter->WriteEndElement();
}

void DebugInfoXmlWriter::WriteChildProcessMap(const Debug::DebugeeInfo& debuggeeInfo, const std::multimap<DWORD, DWORD>& mmapChildProcesses)
{
   m_spWriter->WriteStartElement(_T("child-process-map"));
   m_spWriter->WriteStartElementEnd();

   WriteChildProcessEntry(debuggeeInfo, mmapChildProcesses, 0);

   m_spWriter->WriteEndElement();
}

void DebugInfoXmlWriter::WriteChildProcessEntry(const Debug::DebugeeInfo& debuggeeInfo, const std::multimap<DWORD, DWORD>& mmapChildProcesses, DWORD dwProcessId)
{
   m_spWriter->WriteStartElement(_T("process"));

   if (dwProcessId == 0)
      m_spWriter->WriteAttributeString(_T("image-name"), _T("debugger"));
   else
   {
      CString cszText; cszText.Format(_T("0x%08x"), dwProcessId);
      m_spWriter->WriteAttributeString(_T("pid"), cszText);

      if (debuggeeInfo.IsProcessInfoAvail(dwProcessId))
      {
         const Debug::ProcessInfo processInfo = debuggeeInfo.GetProcessInfo(dwProcessId);
         m_spWriter->WriteAttributeString(_T("image-name"), processInfo.ImageName());
      }
      else
         m_spWriter->WriteAttributeString(_T("image-name"), _T("unknown"));
   }
   m_spWriter->WriteStartElementEnd();

   // find all child entries
   std::multimap<DWORD, DWORD>::const_iterator iter = mmapChildProcesses.find(dwProcessId);
   while (iter != mmapChildProcesses.end() && iter->first == dwProcessId)
   {
      WriteChildProcessEntry(debuggeeInfo, mmapChildProcesses, iter->second);
      ++iter;
   }

   m_spWriter->WriteEndElement();
}

void DebugInfoXmlWriter::WriteExceptionInfo(const Debug::ExceptionInfo& exceptionInfo)
{
   m_spWriter->WriteStartElement(_T("exception-info"));

   CString cszText; cszText.Format(_T("0x%08x"), exceptionInfo.ProcessId());
   m_spWriter->WriteAttributeString(_T("process-id"), cszText);

   cszText.Format(_T("0x%08x"), exceptionInfo.ThreadId());
   m_spWriter->WriteAttributeString(_T("thread-id"), cszText);

   cszText.Format(_T("0x%08x"), exceptionInfo.ExceptionCode());
   m_spWriter->WriteAttributeString(_T("exception-code"), cszText);

   cszText.Format(_T("0x%p"), exceptionInfo.ExceptionAddress());
   m_spWriter->WriteAttributeString(_T("exception-address"), cszText);

   m_spWriter->WriteAttributeString(_T("type"), exceptionInfo.IsFirstChance() ? _T("1st-chance") : _T("2nd-chance"));

   m_spWriter->WriteStartElementEnd();

   {
      m_spWriter->WriteStartElement(_T("exception-numbers"));
      m_spWriter->WriteStartElementEnd();

      const std::vector<ULONG_PTR>& vecNumbers = exceptionInfo.ExceptionInformation();

      CString cszNumber;
      for (size_t i=0, iMax=vecNumbers.size(); i<iMax; i++)
      {
         cszNumber.Format(_T("0x%lx"), vecNumbers[i]);
         m_spWriter->WriteElementString(_T("number"), cszNumber);
      }

      m_spWriter->WriteEndElement();
   }

   // callstack
   const Debug::CallStack& callstack = exceptionInfo.Callstack();
   WriteCallStack(callstack);

   m_spWriter->WriteEndElement();
}

void DebugInfoXmlWriter::WriteCallStack(const Debug::CallStack& callstack)
{
   m_spWriter->WriteStartElement(_T("callstack"));

   CString cszText; cszText.Format(_T("%u"), callstack.GetSize());
   m_spWriter->WriteAttributeString(_T("frame-count"), cszText);

   m_spWriter->WriteStartElementEnd();

   // output each frame
   for (size_t i=0, iMax=callstack.GetSize(); i<iMax; i++)
   {
      const Debug::FunctionCall& functionCall = callstack.GetFunctionCall(i);

      WriteFunctionCall(i, functionCall);
   }

   m_spWriter->WriteEndElement();
}

void DebugInfoXmlWriter::WriteFunctionCall(size_t iIndex, const Debug::FunctionCall& functionCall)
{
   m_spWriter->WriteStartElement(_T("frame"));

   CString cszText; cszText.Format(_T("%u"), iIndex);
   m_spWriter->WriteAttributeString(_T("nr"), cszText);

   cszText.Format(_T("0x%0I64x"), functionCall.GetIP());
   m_spWriter->WriteAttributeString(_T("ip"), cszText);

   cszText.Format(_T("0x%0I64x"), functionCall.ModuleBaseAddress());
   m_spWriter->WriteAttributeString(_T("module-base-address"), cszText);

   m_spWriter->WriteAttributeString(_T("function-name"), functionCall.GetFunctionName());

   cszText.Format(_T("+0x%I64x"), functionCall.GetFunctionDisplacement());
   m_spWriter->WriteAttributeString(_T("displacement"), cszText);

   if (functionCall.SourceFilename().IsEmpty() && functionCall.SourceLineNumber() == 0)
      cszText = _T("N/A");
   else
      cszText.Format(_T("%s(%u)"), functionCall.SourceFilename().GetString(), functionCall.SourceLineNumber());
   m_spWriter->WriteAttributeString(_T("sourcecode"), cszText);

   //m_spWriter->WriteStartElementEnd();


   m_spWriter->WriteEndElement();
}

void DebugInfoXmlWriter::WriteDebuggeeInfo(const Debug::DebugeeInfo& debuggeeInfo)
{
   m_spWriter->WriteStartElement(_T("running-processes"));
   m_spWriter->WriteStartElementEnd();
   {
      const Debug::DebugeeInfo::T_mapCurrentProcesses& mapRunningProcesses = debuggeeInfo.GetRunningProcesses();

      Debug::DebugeeInfo::T_mapCurrentProcesses::const_iterator iter = mapRunningProcesses.begin(), stop = mapRunningProcesses.end();
      for (;iter != stop; iter++)
         WriteProcessInfo(iter->second);
   }
   m_spWriter->WriteEndElement();

   m_spWriter->WriteStartElement(_T("abandoned-processes"));
   m_spWriter->WriteStartElementEnd();
   {
      const Debug::DebugeeInfo::T_setAbandonedProcesses& setAbandonedProcesses = debuggeeInfo.GetAbandonedProcesses();

      Debug::DebugeeInfo::T_setAbandonedProcesses::const_iterator iter = setAbandonedProcesses.begin(), stop = setAbandonedProcesses.end();
      for (;iter != stop; iter++)
         WriteProcessInfo(*iter);
   }
   m_spWriter->WriteEndElement();
}

void DebugInfoXmlWriter::WriteProcessInfo(const Debug::ProcessInfo& processInfo)
{
   m_spWriter->WriteStartElement(_T("process"));

   CString cszText; cszText.Format(_T("0x%08x"), processInfo.ProcessId());
   m_spWriter->WriteAttributeString(_T("pid"), cszText);

   m_spWriter->WriteAttributeString(_T("image-name"), processInfo.ImageName());

   m_spWriter->WriteAttributeString(_T("image-version"), processInfo.ImageVersion());

   cszText.Format(_T("0x%p"), processInfo.ImageAddress());
   m_spWriter->WriteAttributeString(_T("image-addr"), cszText);

   cszText.Format(_T("0x%08x"), processInfo.ExitCode());
   m_spWriter->WriteAttributeString(_T("exit-code"), cszText);

   WriteTimeAttribute(_T("start-time"), processInfo.StartTime());
   WriteTimeAttribute(_T("stop-time"), processInfo.StopTime());

   m_spWriter->WriteStartElementEnd();

   // write all threads
   m_spWriter->WriteStartElement(_T("threads"));
   m_spWriter->WriteStartElementEnd();
   {
      m_spWriter->WriteStartElement(_T("running-threads"));
      m_spWriter->WriteStartElementEnd();
      {
         const Debug::ProcessInfo::T_mapRunningThreads& mapCurrentThreads = processInfo.GetAllRunningThreads();
         Debug::ProcessInfo::T_mapRunningThreads::const_iterator iter = mapCurrentThreads.begin(), stop = mapCurrentThreads.end();
         for (; iter != stop; iter++)
            WriteThreadInfo(iter->second);
      }
      m_spWriter->WriteEndElement();

      m_spWriter->WriteStartElement(_T("abandoned-threads"));
      m_spWriter->WriteStartElementEnd();
      {
         const Debug::ProcessInfo::T_setAbandonedThreads& setAbandonedThreads = processInfo.GetAllAbandonedThreads();
         Debug::ProcessInfo::T_setAbandonedThreads::const_iterator iter = setAbandonedThreads.begin(), stop = setAbandonedThreads.end();
         for (; iter != stop; iter++)
            WriteThreadInfo(*iter);
      }
      m_spWriter->WriteEndElement();
   }
   m_spWriter->WriteEndElement();

   // write all modules
   m_spWriter->WriteStartElement(_T("modules"));
   m_spWriter->WriteStartElementEnd();
   {
      m_spWriter->WriteStartElement(_T("loaded-modules"));
      m_spWriter->WriteStartElementEnd();
      {
         const Debug::ProcessInfo::T_mapLoadedModules& mapCurrentModules = processInfo.GetAllLoadedModules();
         Debug::ProcessInfo::T_mapLoadedModules::const_iterator iter = mapCurrentModules.begin(), stop = mapCurrentModules.end();
         for (; iter != stop; iter++)
            WriteModuleInfo(iter->second);
      }
      m_spWriter->WriteEndElement();

      m_spWriter->WriteStartElement(_T("unloaded-modules"));
      m_spWriter->WriteStartElementEnd();
      {
         const Debug::ProcessInfo::T_setUnloadedModules& setUnloadedModules = processInfo.GetAllUnloadedModules();
         Debug::ProcessInfo::T_setUnloadedModules::const_iterator iter = setUnloadedModules.begin(), stop = setUnloadedModules.end();
         for (; iter != stop; iter++)
            WriteModuleInfo(*iter);
      }
      m_spWriter->WriteEndElement();
   }
   m_spWriter->WriteEndElement();

   m_spWriter->WriteEndElement();
}

void DebugInfoXmlWriter::WriteThreadInfo(const Debug::ThreadInfo& threadInfo)
{
   m_spWriter->WriteStartElement(_T("thread"));

   CString cszText; cszText.Format(_T("0x%08x"), threadInfo.ThreadId());
   m_spWriter->WriteAttributeString(_T("tid"), cszText);

   cszText.Format(_T("0x%p"), threadInfo.StartProc());
   m_spWriter->WriteAttributeString(_T("start-proc"), cszText);

   cszText.Format(_T("0x%08x"), threadInfo.ExitCode());
   m_spWriter->WriteAttributeString(_T("exit-code"), cszText);

   WriteTimeAttribute(_T("start-time"), threadInfo.StartTime());
   WriteTimeAttribute(_T("stop-time"), threadInfo.StopTime());

   m_spWriter->WriteStartElementEnd();

   m_spWriter->WriteEndElement();
}

void DebugInfoXmlWriter::WriteModuleInfo(const Debug::ModuleInfo& moduleInfo)
{
   m_spWriter->WriteStartElement(_T("module"));

   CString cszText; cszText.Format(_T("0x%p"), moduleInfo.BaseAddress());
   m_spWriter->WriteAttributeString(_T("base-address"), cszText);

   cszText.Format(_T("0x%08x"), static_cast<DWORD>(moduleInfo.Size()));
   m_spWriter->WriteAttributeString(_T("size"), cszText);

   WriteTimeAttribute(_T("load-time"), moduleInfo.LoadTime());
   WriteTimeAttribute(_T("unload-time"), moduleInfo.UnloadTime());

   m_spWriter->WriteAttributeString(_T("module-name"), moduleInfo.ModuleName());

   m_spWriter->WriteAttributeString(_T("file-version"), moduleInfo.FileVersion());

   m_spWriter->WriteStartElementEnd();

   m_spWriter->WriteEndElement();
}

void DebugInfoXmlWriter::WriteTimeAttribute(const CString& cszAttributeName, const SystemTime& time)
{
   if (time.IsEmpty())
   {
      m_spWriter->WriteAttributeString(cszAttributeName, _T("N/A"));
   }
   else
   {
      DateTime dt(time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, time.wMilliseconds);

      m_spWriter->WriteAttributeString(cszAttributeName, dt.FormatISO8601(DateTime::formatYMD_HMSF_Z, false));
   }
}
