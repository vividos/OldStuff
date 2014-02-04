//
// MrCrash - crash info reporting tool
// Copyright (C) 2005-2014 Michael Fink
//
/// \file ThreadInfo.hpp Thread info
//
#pragma once

// includes
#include <memory>
#include <ulib/win32/SystemTime.hpp>

namespace Debug
{

/// \brief thread infos
/// data is shared between copies of this object
class ThreadInfo
{
public:
   ThreadInfo(DWORD dwThreadId)
      :m_spData(new ThreadInfoData(dwThreadId))
   {
   }

   // get/set methods

   DWORD ThreadId() const throw() { return m_spData->m_dwThreadId; }
   LPVOID StartProc() const throw() { return m_spData->m_pStartProc; }
   DWORD ExitCode() const throw() { return m_spData->m_dwExitCode; }
   SystemTime StartTime() const throw() { return m_spData->m_stStartTime; }
   SystemTime StopTime() const throw() { return m_spData->m_stStopTime; }

   void StartProc(LPVOID pStartProc) throw() { m_spData->m_pStartProc = pStartProc; }
   void StartTime(SystemTime stStartTime) throw() { m_spData->m_stStartTime = stStartTime; }
   void StopTime(SystemTime stStopTime) throw() { m_spData->m_stStopTime = stStopTime; }
   void ExitCode(DWORD dwExitCode) throw() { m_spData->m_dwExitCode = dwExitCode; }

   /// less-than operator to sort ThreadInfo in an ordered container
   bool operator<(const ThreadInfo& pi) const
   {
      return m_spData->m_dwThreadId < pi.m_spData->m_dwThreadId;
   }

   /// less functor to sort ThreadInfo's by start time in an ordered container
   class CSortThreadInfoByStartTime: public std::less<ThreadInfo>
   {
   public:
      bool operator()(const ThreadInfo& ti1, const ThreadInfo& ti2) const throw()
      {
         return ti1.m_spData->m_stStartTime < ti2.m_spData->m_stStartTime;
      }
   };

private:
   /// thread data struct
   struct ThreadInfoData
   {
      ThreadInfoData(DWORD dwThreadId)
         :m_dwThreadId(dwThreadId),
          m_pStartProc(NULL),
          m_dwExitCode(0)
      {
      }

      DWORD m_dwThreadId;
      LPVOID m_pStartProc;
      DWORD m_dwExitCode;
      SystemTime m_stStartTime;
      SystemTime m_stStopTime;
   };

   /// thread data
   std::shared_ptr<ThreadInfoData> m_spData;
};

} // namespace Debug
