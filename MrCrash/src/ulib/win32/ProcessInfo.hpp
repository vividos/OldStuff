//
// ulib - a collection of useful classes
// Copyright (C) 2006-2014 Michael Fink
//
/// \file ProcessInfo.hpp Process info
//
#pragma once

namespace Win32
{

/// process info
class ProcessInfo
{
public:
   /// ctor
   ProcessInfo(DWORD dwProcessId)
      :m_dwProcessId(dwProcessId)
   {
   }

   /// returns user and domain name that a process is running under
   bool GetUserAndDomainName(CString& cszUsername, CString& cszRefDomainName);

private:
   /// process
   DWORD m_dwProcessId;
};

} // namespace Win32
