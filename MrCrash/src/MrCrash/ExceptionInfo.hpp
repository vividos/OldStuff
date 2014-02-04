//
// MrCrash - crash info reporting tool
// Copyright (C) 2005-2014 Michael Fink
//
/// \file ExceptionInfo.hpp Exception info
//
#pragma once

// includes
#include <ulib/debug/CallStack.hpp>
#include <vector>

namespace Debug
{

/// \brief stores exception info
/// \note there could be more than one exception record in EXCEPTION_DEBUG_INFO, but this
/// this class only stores the first one
class ExceptionInfo
{
public:
   /// ctor
   ExceptionInfo(DWORD dwProcessId, DWORD dwThreadId, EXCEPTION_DEBUG_INFO& debugInfo)
      :m_dwProcessId(dwProcessId),
       m_dwThreadId(dwThreadId),
       m_dwExceptionCode(debugInfo.ExceptionRecord.ExceptionCode),
       m_pExceptionAddress(debugInfo.ExceptionRecord.ExceptionAddress),
       m_bFirstChance(debugInfo.dwFirstChance == 1),
       m_callStack(dwProcessId, dwThreadId)
   {
      // add exception numbers
      for (DWORD dw=0; dw<debugInfo.ExceptionRecord.NumberParameters; dw++)
         m_vecExceptionInformation.push_back(debugInfo.ExceptionRecord.ExceptionInformation[dw]);
   }

   /// adds call stack for exception
   void AddCallStack(Debug::CallStack& callStack)
   {
      m_callStack = callStack;
   }

   /// process id where exception occured
   DWORD ProcessId() const throw() { return m_dwProcessId; }

   /// thread id where exception occured
   DWORD ThreadId() const throw() { return m_dwThreadId; }

   /// exception code, e.g. 0xC0000005
   DWORD ExceptionCode() const throw() { return m_dwExceptionCode; }

   /// returns if it's a first chance exception (else it's a second chance exception)
   bool IsFirstChance() const throw(){ return m_bFirstChance; }

   /// returns exception address
   PVOID ExceptionAddress() const throw() { return m_pExceptionAddress; }

   /// returns exception infos
   const std::vector<ULONG_PTR>& ExceptionInformation() const throw() { return m_vecExceptionInformation; }

   /// returns callstack
   const Debug::CallStack& Callstack() const throw() { return m_callStack; }

private:
   DWORD m_dwProcessId;
   DWORD m_dwThreadId;
   DWORD m_dwExceptionCode;
   PVOID m_pExceptionAddress;
   bool m_bFirstChance;
   std::vector<ULONG_PTR> m_vecExceptionInformation;
   Debug::CallStack m_callStack;
};

} // namespace Debug
