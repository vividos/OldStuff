//
// Recorder - a GPS logger app for Windows Mobile
// Copyright (C) 2006-2019 Michael Fink
//
/// \file WorkerThread.hpp worker thread class
//
#pragma once

#include <boost/function.hpp>
#include <stdexcept>

#ifdef WIN32

#ifndef MS_VC_EXCEPTION
#define MS_VC_EXCEPTION 0x406D1388
#endif

#pragma pack(push,8)
typedef struct tagTHREADNAME_INFO
{
   DWORD dwType; // Must be 0x1000.
   LPCSTR szName; // Pointer to name (in user addr space).
   DWORD dwThreadID; // Thread ID (-1=caller thread).
   DWORD dwFlags; // Reserved for future use, must be zero.
} THREADNAME_INFO;
#pragma pack(pop)

#endif // WIN32


/// worker thread
class WorkerThread
{
public:
   /// thread function typedef
   typedef boost::function<int()> T_fnThread;

   /// ctor
   WorkerThread(T_fnThread fnThread)
      :m_fnThread(fnThread),
      m_dwThreadId(0),
      m_evtStarted(true, false) // manual-reset event
   {
      HANDLE hThread = ::CreateThread(NULL, 0, ThreadProc, this, 0, &m_dwThreadId);
      if (hThread == NULL)
         throw std::runtime_error("failed to create thread");

      m_spThread.reset(hThread, ::CloseHandle);

      // wait for thread start
      WaitForSingleObject(m_evtStarted, INFINITE);
   }

   /// sets thread name
   /// \note from http://msdn.microsoft.com/en-us/library/xcb2z8hs.aspx
   void SetName(LPCTSTR pszThreadName)
   {
#if defined(_WIN32) && !defined(_WIN32_WCE)
      USES_CONVERSION;

      THREADNAME_INFO info;
      info.dwType = 0x1000;
      info.szName = T2CA(pszThreadName);
      info.dwThreadID = Id();
      info.dwFlags = 0;

      __try
      {
         RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR*)& info);
      }
      __except (EXCEPTION_EXECUTE_HANDLER)
      {
      }
#endif // WIN32
   }

   /// joins thread
   void Join()
   {
      ATLASSERT(m_spThread.get() != 0);
      DWORD dwRet = ::WaitForSingleObject(m_spThread.get(), INFINITE);
      if (dwRet == WAIT_OBJECT_0)
         return;
      throw std::runtime_error("failed to join worker thread");
   }

   /// returns id
   DWORD Id() const throw() { return m_dwThreadId; }

   /// returns current thread object
   static WorkerThread Current()
   {
      return WorkerThread(::GetCurrentThreadId());
   }

private:
   /// thread procedure
   static DWORD WINAPI ThreadProc(LPVOID pData)
   {
      static_cast<WorkerThread*>(pData)->m_evtStarted.Set();
      return static_cast<WorkerThread*>(pData)->m_fnThread();
   }

   /// ctor for already running thread
   WorkerThread(DWORD dwThreadId)
      :m_dwThreadId(dwThreadId),
      m_evtStarted(true, false) // manual-reset event
   {
      m_evtStarted.Set();
   }

private:
   /// thread function
   T_fnThread m_fnThread;

   /// thread handle
   boost::shared_ptr<void> m_spThread;

   /// thread ID
   DWORD m_dwThreadId;

   /// event that gets signaled when thread has started
   CEvent m_evtStarted;
};
