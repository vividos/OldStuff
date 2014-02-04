//
// MrCrash - crash info reporting tool
// Copyright (C) 2005-2014 Michael Fink
//
/// \file DebugInfoXmlWriter.hpp Xml writer for debug infos
//
#pragma once

// includes
#include <ulib/xml/XmlWriter.hpp>
#include <map>

// forward references
namespace Debug
{
   class DebugeeInfo;
   class ExceptionInfo;
   class CallStack;
   class FunctionCall;
   class ProcessInfo;
   class ThreadInfo;
   class ModuleInfo;
}
class DebugEvent;
class SystemTime;

class DebugInfoXmlWriter
{
public:
   DebugInfoXmlWriter(XML::XmlWriterPtr spWriter) throw()
      :m_spWriter(spWriter)
   {
   }

   /// writes exception report
   void WriteExceptionReport(
      const Debug::DebugeeInfo& debuggeeInfo,
      const Debug::ExceptionInfo& exceptionInfo,
      const std::multimap<DWORD, DWORD>& mmapChildProcesses,
      const std::vector<DebugEvent>& vecDebugEvents)
   {
      WriteExceptionInfo(exceptionInfo);
      WriteDebuggeeInfo(debuggeeInfo);
      WriteChildProcessMap(debuggeeInfo, mmapChildProcesses);
      WriteDebugEventList(vecDebugEvents);
   }

   /// writes standard report
   void WriteStandardReport(
      const Debug::DebugeeInfo& debuggeeInfo,
      const std::multimap<DWORD, DWORD>& mmapChildProcesses,
      const std::vector<DebugEvent>& vecDebugEvents)
   {
      WriteDebuggeeInfo(debuggeeInfo);
      WriteChildProcessMap(debuggeeInfo, mmapChildProcesses);
      WriteDebugEventList(vecDebugEvents);
   }

   void WriteDebugEventList(const std::vector<DebugEvent>& vecDebugEvents);
   void WriteDebugEvent(size_t iIndex, const DebugEvent& debugEvent);
   void WriteChildProcessMap(const Debug::DebugeeInfo& debuggeeInfo, const std::multimap<DWORD, DWORD>& mmapChildProcesses);
   void WriteChildProcessEntry(const Debug::DebugeeInfo& debuggeeInfo, const std::multimap<DWORD, DWORD>& mmapChildProcesses, DWORD dwProcessId);
   void WriteExceptionInfo(const Debug::ExceptionInfo& exceptionInfo);
   void WriteCallStack(const Debug::CallStack& callstack);
   void WriteFunctionCall(size_t iIndex, const Debug::FunctionCall& functionCall);
   void WriteDebuggeeInfo(const Debug::DebugeeInfo& debuggeeInfo);
   void WriteProcessInfo(const Debug::ProcessInfo& processInfo);
   void WriteThreadInfo(const Debug::ThreadInfo& threadInfo);
   void WriteModuleInfo(const Debug::ModuleInfo& moduleInfo);
   void WriteTimeAttribute(const CString& cszAttributeName, const SystemTime& time);

private:
   /// xml writer to use
   XML::XmlWriterPtr m_spWriter;
};
