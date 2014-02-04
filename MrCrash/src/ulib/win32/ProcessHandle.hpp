//
// ulib - a collection of useful classes
// Copyright (C) 2006-2014 Michael Fink
//
/// \file ProcessHandle.hpp process handle wrapper
//
#pragma once

namespace Win32
{

/// process handle wrapper
class ProcessHandle
{
public:
   /// ctor; opens handle from pid with desired access
   ProcessHandle(DWORD dwProcessID, DWORD dwDesiredAccess) throw()
      :m_hProcess(::OpenProcess(dwDesiredAccess, FALSE, dwProcessID))
   {
   }

   /// dtor; auto-closes handle
   ~ProcessHandle() throw()
   {
      CloseHandle(m_hProcess);
   }

   // conversion operators

   operator HANDLE() const throw() { return m_hProcess; }
   operator HANDLE()throw() { return m_hProcess; }

private:
   HANDLE m_hProcess;   ///< process handle
};

} // namespace Win32
