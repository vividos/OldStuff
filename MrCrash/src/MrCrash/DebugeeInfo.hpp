//
// MrCrash - crash info reporting tool
// Copyright (C) 2005-2014 Michael Fink
//
/// \file DebugeeInfo.hpp Info about debugged process
//
#pragma once

// includes
#include <boost/noncopyable.hpp>
#include <map>
#include <set>
#include "ProcessInfo.hpp"

namespace Debug
{

/// contains infos about all debugged processes
class DebugeeInfo: public boost::noncopyable
{
public:
   DebugeeInfo()
   {
   }

   typedef std::map<DWORD, ProcessInfo> T_mapCurrentProcesses;
   typedef std::set<ProcessInfo, ProcessInfo::CSortProcessInfoByStartTime> T_setAbandonedProcesses;

   /// returns if process info for a given process id is available
   bool IsProcessInfoAvail(DWORD dwProcessId) const
   {
      return m_mapCurrentProcesses.find(dwProcessId) != m_mapCurrentProcesses.end();
   }

   /// returns process info by process id
   ProcessInfo GetProcessInfo(DWORD dwProcessId)
   {
      ATLASSERT(true == IsProcessInfoAvail(dwProcessId));

      std::map<DWORD, ProcessInfo>::iterator iter =
         m_mapCurrentProcesses.find(dwProcessId);

      ATLASSERT(dwProcessId == iter->second.ProcessId());

      return iter->second;
   }

   /// returns process info by process id; const version
   const ProcessInfo GetProcessInfo(DWORD dwProcessId) const
   {
      ATLASSERT(true == IsProcessInfoAvail(dwProcessId));

      std::map<DWORD, ProcessInfo>::const_iterator iter =
         m_mapCurrentProcesses.find(dwProcessId);

      ATLASSERT(dwProcessId == iter->second.ProcessId());

      return iter->second;
   }

   /// adds new process
   void AddProcess(ProcessInfo pi)
   {
      m_mapCurrentProcesses.insert(std::make_pair(pi.ProcessId(), pi));
   }

   /// abandons process
   void AbandonProcess(DWORD dwProcessId)
   {
      ATLASSERT(true == IsProcessInfoAvail(dwProcessId));

      ProcessInfo pi = GetProcessInfo(dwProcessId);
      m_setAbandonedProcesses.insert(pi);
      m_mapCurrentProcesses.erase(dwProcessId);
   }

   /// returns current process count
   size_t GetCurrentProcessCount()
   {
      return m_mapCurrentProcesses.size();
   }

   const T_mapCurrentProcesses& GetRunningProcesses() const throw()
   {
      return m_mapCurrentProcesses;
   }

   const T_setAbandonedProcesses& GetAbandonedProcesses() const throw()
   {
      return m_setAbandonedProcesses;
   }
/*
private:
   // prevent copying
   DebugeeInfo(const DebugeeInfo&);
   DebugeeInfo& operator=(const DebugeeInfo&);
*/
private:
   /// map with all currently running processes
   T_mapCurrentProcesses m_mapCurrentProcesses;


   /// set with all abandoned processes
   T_setAbandonedProcesses m_setAbandonedProcesses;
};

} // namespace Debug
