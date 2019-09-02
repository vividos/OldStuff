#pragma once

#include <ulib/mt/WorkerThread.hpp>

/// \brief class that implements a worker thread that waits on a serial port for read/write events
/// the class is mainly written for Windows CE when no overlapped support is available; waiting
/// for events is simulated using calling WaitCommEvent() in another thread waiting for an event
/// that gets set when the call returns.
class CWaitEventThread: public CWorkerThread<CWaitEventThread>
{
public:
   /// ctor
   CWaitEventThread(HANDLE hPort)
      :m_hPort(hPort),
      m_hEventReady(::CreateEvent(NULL, TRUE, FALSE, NULL)), // manual-reset
      m_hEventWait(::CreateEvent(NULL, TRUE, FALSE, NULL)), // manual-reset
      m_hEventContinue(::CreateEvent(NULL, TRUE, FALSE, NULL)), // manual-reset
      m_hEventStop(::CreateEvent(NULL, TRUE, FALSE, NULL)) // manual-reset
   {
      Start();
   }

   ~CWaitEventThread()
   {
      // stop background thread
      StopThread();
      ::WaitForSingleObject(m_hThread, INFINITE);

      CloseHandle(m_hEventReady);
      CloseHandle(m_hEventWait);
      CloseHandle(m_hEventContinue);
      CloseHandle(m_hEventStop);
   }

   /// \todo only in ready state?
   void SetPort(HANDLE hPort)
   {
      // TODO only in "ready" state?
      m_hPort = hPort;
   }

   /// \todo thread-safe exchange?
   void SetMask(DWORD dwEventMask)
   {
      //ATLTRACE(_T("T1: setting new mask, %08x\n"), dwEventMask);
      // TODO thread-safe exchange? only do in "ready" thread state?
      m_dwEventMask = dwEventMask;
   }

   /// waits for thead to be ready to wait for comm event
   void WaitForThreadReady()
   {
      WaitForSingleObject(m_hEventReady, INFINITE);
      ResetEvent(m_hEventReady);
   }

   /// tells thread to continue waiting
   void ContinueWait()
   {
      SetEvent(m_hEventContinue);
   }

   /// \todo wait for ready thread state
   void StopThread()
   {
      // TODO wait for "ready" thread state
      //ATLTRACE(_T("T1: stopping worker thread\n"));
      ::SetEvent(m_hEventStop);

      if (FALSE == ::SetCommMask(m_hPort, 0))
      {
         DWORD dw = GetLastError();
         dw++;
      }
      //ATLTRACE(_T("T1: stopped worker thread\n"));
   }

   bool WaitComm();

   int Run();

   /// returns wait event handle
   HANDLE GetWaitEventHandle() const { return m_hEventWait; }

   /// returns event result mask
   DWORD GetEventResult()
   {
      // wait event must be set
      //ATLTRACE(_T("T2: testing wait event\n"));
      DWORD dwTest = ::WaitForSingleObject(m_hEventWait, 0);
      //ATLASSERT(WAIT_OBJECT_0 == dwTest);

      // note: m_dwEventResult can be accessed without protection from the worker
      //       thread because it is set when WaitCommEvent() returned.
      return m_dwEventResult;
   }

private:
   DWORD m_dwEventMask;
   DWORD m_dwEventResult;
   HANDLE m_hPort;

   //! when event is set, the wait thread is ready to do a next "wait" task
   HANDLE m_hEventReady;

   //! when event is set, the WaitCommEvent call returned successfully
   HANDLE m_hEventWait;

   //! when event is set, the wait thread is allowed to continue with a WaitCommEvent call
   HANDLE m_hEventContinue;

   //! when event is set, the wait thread should exit
   HANDLE m_hEventStop;
};
