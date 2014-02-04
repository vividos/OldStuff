//
// MrCrash - crash info reporting tool
// Copyright (C) 2005-2014 Michael Fink
//
/// \file ProcessInfo.hpp Process info
//
#pragma once

// includes
#include <map>
#include <set>
#include <memory>
#include "ThreadInfo.hpp"
#include "ModuleInfo.hpp"
#include <ulib/win32/SystemTime.hpp>
#include <ulib/debug/SymbolManager.hpp>
#include <ulib/win32/ProcessHandle.hpp>

namespace Debug
{

/// \brief process infos
/// data is shared between copies of this object
class ProcessInfo
{
public:
   ProcessInfo(DWORD dwProcessId)
      :m_spData(new ProcessInfoData(dwProcessId))
   {
   }

   typedef std::map<DWORD, ThreadInfo> T_mapRunningThreads;
   typedef std::multiset<ThreadInfo, ThreadInfo::CSortThreadInfoByStartTime> T_setAbandonedThreads;

   typedef std::map<LPVOID, ModuleInfo> T_mapLoadedModules;
   typedef std::multiset<ModuleInfo, ModuleInfo::CSortLibraryInfoByLoadTime> T_setUnloadedModules;

   // get/set methods

   DWORD ProcessId() const throw() { return m_spData->m_dwProcessId; }
   const CString& ImageName() const throw() { return m_spData->m_cszImageName; }
   const CString& ImageVersion() const throw() { return m_spData->m_cszImageVersion; }
   LPVOID ImageAddress() const throw() { return m_spData->m_pImageAddr; }
   DWORD ExitCode() const throw() { return m_spData->m_dwExitCode; }
   SystemTime StartTime() const throw() { return m_spData->m_stStartTime; }
   SystemTime StopTime() const throw() { return m_spData->m_stStopTime; }

   SymbolManager& SymbolManager(){ return m_spData->m_symbolManager; }

   void ImageName(const CString& cszImageName) throw() { m_spData->m_cszImageName = cszImageName; }
   void ImageVersion(const CString& cszImageVersion) throw() { m_spData->m_cszImageVersion = cszImageVersion; }
   void ImageAddress(LPVOID pImageAddr) throw() { m_spData->m_pImageAddr = pImageAddr; }
   void ExitCode(DWORD dwExitCode) throw() { m_spData->m_dwExitCode = dwExitCode; }
   void StartTime(SystemTime stStartTime) throw() { m_spData->m_stStartTime = stStartTime; }
   void StopTime(SystemTime stStopTime) throw() { m_spData->m_stStopTime = stStopTime; }

   // thread info methods

   /// returns if thread info for a given thread id is available
   bool IsThreadInfoAvail(DWORD dwThreadId) const
   {
      ATLASSERT(m_spData != NULL);
      return m_spData->m_mapCurrentThreads.find(dwThreadId) != m_spData->m_mapCurrentThreads.end();
   }

   /// returns thread info for given thread id in the process
   ThreadInfo GetThreadInfo(DWORD dwThreadId)
   {
      ATLASSERT(m_spData != NULL);
      ATLASSERT(true == IsThreadInfoAvail(dwThreadId));

      T_mapRunningThreads::iterator iter =
         m_spData->m_mapCurrentThreads.find(dwThreadId);

      ATLASSERT(dwThreadId == iter->second.ThreadId());

      return iter->second;
   }

   /// adds new thread
   void AddThread(ThreadInfo ti)
   {
      ATLASSERT(m_spData != NULL);
      m_spData->m_mapCurrentThreads.insert(std::make_pair(ti.ThreadId(), ti));
   }

   /// marks thread as abandoned; thread must have been added before
   void AbandonThread(DWORD dwThreadId)
   {
      ATLASSERT(m_spData != NULL);
      ATLASSERT(true == IsThreadInfoAvail(dwThreadId));

      ThreadInfo ti = GetThreadInfo(dwThreadId);
      m_spData->m_setAbandonedThreads.insert(ti);
      m_spData->m_mapCurrentThreads.erase(dwThreadId);
   }

   /// abandons all threads (e.g. when process ends)
   void AbandonAllThreads(const SystemTime& stStopTime)
   {
      ATLASSERT(m_spData != NULL);

      T_mapRunningThreads::iterator iter = m_spData->m_mapCurrentThreads.begin(),
         stop = m_spData->m_mapCurrentThreads.end();

      for(;iter != stop; iter++)
      {
         iter->second.StopTime(stStopTime);
         m_spData->m_setAbandonedThreads.insert(iter->second);
      }

      m_spData->m_mapCurrentThreads.clear();
   }

   /// returns map with all running threads
   T_mapRunningThreads& GetAllRunningThreads() throw()
   {
      ATLASSERT(m_spData != NULL);
      return m_spData->m_mapCurrentThreads;
   }

   /// returns map with all running threads; const-version
   const T_mapRunningThreads& GetAllRunningThreads() const throw()
   {
      ATLASSERT(m_spData != NULL);
      return m_spData->m_mapCurrentThreads;
   }

   /// returns set with all abandoned threads
   T_setAbandonedThreads& GetAllAbandonedThreads() throw()
   {
      ATLASSERT(m_spData != NULL);
      return m_spData->m_setAbandonedThreads;
   }

   /// returns set with all abandoned threads; const-version
   const T_setAbandonedThreads& GetAllAbandonedThreads() const throw()
   {
      ATLASSERT(m_spData != NULL);
      return m_spData->m_setAbandonedThreads;
   }

   // module info methods

   /// returns if module info for a given module image base address is available
   bool IsModuleInfoAvail(LPVOID pBaseAddress) const
   {
      ATLASSERT(m_spData != NULL);
      return m_spData->m_mapCurrentModules.find(pBaseAddress) != m_spData->m_mapCurrentModules.end();
   }

   /// returns module info for a given module image base address
   ModuleInfo GetModuleInfo(LPVOID pBaseAddress)
   {
      ATLASSERT(m_spData != NULL);
      ATLASSERT(true == IsModuleInfoAvail(pBaseAddress));

      T_mapLoadedModules::iterator iter =
         m_spData->m_mapCurrentModules.find(pBaseAddress);

      ATLASSERT(pBaseAddress == iter->second.BaseAddress());

      return iter->second;
   }

   /// adds new module
   void AddModule(ModuleInfo mi)
   {
      ATLASSERT(m_spData != NULL);
      m_spData->m_mapCurrentModules.insert(std::make_pair(mi.BaseAddress(), mi));
   }

   /// unloads a module
   void UnloadModule(LPVOID pBaseAddress)
   {
      ATLASSERT(m_spData != NULL);
      ATLASSERT(true == IsModuleInfoAvail(pBaseAddress));

      ModuleInfo mi = GetModuleInfo(pBaseAddress);
      m_spData->m_setUnloadedModules.insert(mi);
      m_spData->m_mapCurrentModules.erase(pBaseAddress);
   }

   /// unloads all modules
   void UnloadAllModules(const SystemTime& stUnloadTime)
   {
      ATLASSERT(m_spData != NULL);
      T_mapLoadedModules::iterator iter = m_spData->m_mapCurrentModules.begin(),
         stop = m_spData->m_mapCurrentModules.end();

      for(;iter != stop; iter++)
      {
         iter->second.UnloadTime(stUnloadTime);
         m_spData->m_setUnloadedModules.insert(iter->second);
      }

      m_spData->m_mapCurrentModules.clear();
   }

   /// returns all modules currently loaded
   T_mapLoadedModules& GetAllLoadedModules() throw()
   {
      ATLASSERT(m_spData != NULL);
      return m_spData->m_mapCurrentModules;
   }

   /// returns all modules currently loaded; const-version
   const T_mapLoadedModules& GetAllLoadedModules() const throw()
   {
      ATLASSERT(m_spData != NULL);
      return m_spData->m_mapCurrentModules;
   }

   /// returns all modules already unloaded
   T_setUnloadedModules& GetAllUnloadedModules() throw()
   {
      ATLASSERT(m_spData != NULL);
      return m_spData->m_setUnloadedModules;
   }

   /// returns all modules already unloaded; const-version
   const T_setUnloadedModules& GetAllUnloadedModules() const throw()
   {
      ATLASSERT(m_spData != NULL);
      return m_spData->m_setUnloadedModules;
   }

   // compare operators

   /// less-than operator to sort ProcessInfo in an ordered container
   bool operator<(const ProcessInfo& pi) const
   {
      // order by process id
      return m_spData->m_dwProcessId < pi.m_spData->m_dwProcessId;
   }

   /// less functor to sort ProcessInfo's by start time in an ordered container
   class CSortProcessInfoByStartTime: public std::less<ProcessInfo>
   {
   public:
      bool operator()(const ProcessInfo& pi1, const ProcessInfo& pi2) const throw()
      {
         return pi1.m_spData->m_stStartTime < pi2.m_spData->m_stStartTime;
      }
   };

private:
   /// process data struct
   struct ProcessInfoData
   {
      ProcessInfoData(DWORD dwProcessId)
         :m_dwProcessId(dwProcessId),
          m_process(dwProcessId, PROCESS_ALL_ACCESS /*PROCESS_QUERY_INFORMATION | PROCESS_VM_READ*/),
          m_pImageAddr(NULL),
          m_dwExitCode(0),
          m_symbolManager(m_process)
      {
      }

      DWORD m_dwProcessId;
      Win32::ProcessHandle m_process;
      CString m_cszImageName;
      CString m_cszImageVersion;
      LPVOID m_pImageAddr;
      DWORD m_dwExitCode;
      SystemTime m_stStartTime;
      SystemTime m_stStopTime;

      Debug::SymbolManager m_symbolManager; // must be below m_process

      /// map of currently running threads
      T_mapRunningThreads m_mapCurrentThreads;

      /// set of abandoned threads, sorted by start time
      T_setAbandonedThreads m_setAbandonedThreads;

      /// map of currently loaded modules
      T_mapLoadedModules m_mapCurrentModules;

      /// set of unloaded modules, sorted by start time
      T_setUnloadedModules m_setUnloadedModules;
   };

   /// process data
   std::shared_ptr<ProcessInfoData> m_spData;
};

} // namespace Debug
