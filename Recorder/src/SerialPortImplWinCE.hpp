//
// Recorder - a GPS logger app for Windows Mobile
// Copyright (C) 2006-2019 Michael Fink
//
/// \file SerialPortImplWinCE.hpp Serial port implementation for WinCE
//
#pragma once

#include "WaitEventThread.hpp"

class CWaitEventThread;

namespace Serial
{

   /// windows CE serial port implementation
   class CSerialPortImpl
   {
   public:
      CSerialPortImpl(HANDLE& hPort)
         :m_hPort(hPort)
      {
      }

      ~CSerialPortImpl()
      {
         m_spWaitEventThread.reset();
      }

      void Open()
      {
         // start background wait thread, when not already running
         if (m_spWaitEventThread.get() == NULL)
            m_spWaitEventThread = boost::shared_ptr<CWaitEventThread>(new CWaitEventThread(m_hPort));
         else
            // already running, so just set new port
            m_spWaitEventThread->SetPort(m_hPort);
      }

      void Close()
      {
         // note: don't end wait thread here, since user could reopen port
      }

      void SetMask(DWORD dwEventMask)
      {
         // also tell wait thread about new mask
         ATLASSERT(m_spWaitEventThread.get() != NULL);
         m_spWaitEventThread->SetMask(dwEventMask);
      }

      bool WaitEvent(DWORD dwTimeout, DWORD dwEventMask, DWORD& dwEventResult, DWORD& dwLastError);

      static bool IsOverlappedSupported() { return false; }

   private:
      //! ref to port handle
      HANDLE& m_hPort;

      // no overlapped support on Windows CE
      boost::shared_ptr<CWaitEventThread> m_spWaitEventThread;
   };

} // namespace Serial


bool Serial::CSerialPortImpl::WaitEvent(DWORD dwTimeout, DWORD dwEventMask, DWORD& dwEventResult, DWORD& dwLastError)
{
   dwLastError = ERROR_SUCCESS;

   ATLASSERT(m_spWaitEventThread.get() != NULL);

   ::SetCommMask(m_hPort, dwEventMask);

   // wait for the "wait" thread to become ready
   m_spWaitEventThread->WaitForThreadReady();

   // reset wait handle
   HANDLE hEventWait = m_spWaitEventThread->GetWaitEventHandle();
   //ATLTRACE(_T("T1: resetting wait event\n"));
   ResetEvent(hEventWait);
   //ATLTRACE(_T("T1: resetting wait event done\n"));

   // must be reset
   ATLASSERT(WAIT_TIMEOUT == ::WaitForSingleObject(hEventWait, 0));

   // tell the wait thread to continue
   m_spWaitEventThread->ContinueWait();

   // wait for a serial port event to occur

   // must be reset
//   ATLASSERT(WAIT_TIMEOUT == ::WaitForSingleObject(hEventWait, 0));

   //ATLTRACE(_T("T1: waiting for wait event\n"));
   DWORD dwT1 = GetTickCount();
   DWORD dwRet = ::WaitForSingleObject(hEventWait, dwTimeout);
   DWORD dwT2 = GetTickCount();
   //ATLTRACE(_T("T1: waiting for wait event done, needed %u ms, timeout was %u ms\n"), dwT2-dwT1, dwTimeout);

   bool bWaitStatus = false;
   switch (dwRet)
   {
   case WAIT_OBJECT_0:
      // must be set
      ATLASSERT(WAIT_OBJECT_0 == ::WaitForSingleObject(hEventWait, 0));

      //ATLTRACE(_T("T1: wait event was got\n"));

      // successfully waited on event
      dwEventResult = m_spWaitEventThread->GetEventResult() & dwEventMask;
      bWaitStatus = dwEventResult != 0;

      ////ATLTRACE(_T("T1: resetting wait event\n"));
      //ResetEvent(hEventWait);
      ////ATLTRACE(_T("T1: resetting wait event done\n"));

      // must be reset
      //ATLASSERT(WAIT_TIMEOUT == ::WaitForSingleObject(hEventWait, 0));

      dwLastError = 0;
      break;

   case WAIT_TIMEOUT:
      dwLastError = ERROR_TIMEOUT;
      break;

   case WAIT_ABANDONED:
   case WAIT_FAILED:
      dwLastError = dwRet;
      break;
   }

   // cancel pending WaitCommEvent() calls
   //ATLTRACE(_T("T1: cancel pending WaitCommEvent calls\n"));
   BOOL bRet = ::SetCommMask(m_hPort, 0);
   if (!bRet)
   {
      //ATLTRACE(_T("T1: error while canceling\n"));
      dwLastError = GetLastError();

      //CloseHandle(m_hPort);

      COMSTAT comstat;
      ZeroMemory(&comstat, sizeof(comstat));
      DWORD dwErrors = 0;

      BOOL bRet = ClearCommError(m_hPort, &dwErrors, &comstat);

      ATLTRACE(_T("ClearCommError: ret=%u, dwErrors=%08x\n"),
         bRet, dwErrors);

      CloseHandle(m_hPort);
   }
   else
   {
      //ATLTRACE(_T("T1: done canceling\n"));
   }

   return bWaitStatus;
}
