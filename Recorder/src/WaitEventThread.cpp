//
// Recorder - a GPS logger app for Windows Mobile
// Copyright (C) 2006-2019 Michael Fink
//
/// \file WaitEventThread.cpp WaitEvent() thread implementation
//
#include "stdafx.h"
#include "WaitEventThread.hpp"

/// note: called from wait thread
bool CWaitEventThread::WaitComm()
{
   // wait handle must not be set
   DWORD dwTest = ::WaitForSingleObject(m_hEventWait, 0);
   ATLASSERT(WAIT_TIMEOUT == dwTest);

   // wait for an event
   //ATLTRACE(_T("T2: calling WaitCommEvent() ...\n"));
   BOOL bRet = ::WaitCommEvent(m_hPort, &m_dwEventResult, NULL);
   //ATLTRACE(_T("T2: returned from WaitCommEvent()\n"));

   if (FALSE == bRet)
   {
      DWORD dwLastError = GetLastError();
      //ATLTRACE(_T("T2: returned error from WaitCommEvent: %08x\n"), dwLastError);

      // event mask was set
      if (m_dwEventResult == 0)
         return true;

      if (dwLastError == ERROR_INVALID_HANDLE)
         return false;
   }

   //ATLTRACE(_T("T2: event result from wait: %08x, mask is %08x\n"), m_dwEventResult, m_dwEventMask);

   return true;
}

int CWaitEventThread::Run()
{
   //ATLTRACE(_T("T2: starting worker thread\n"));

   bool bLoop = true;
   do
   {
      // setting "ready" event
      SetEvent(m_hEventReady);

      //ATLTRACE(_T("T2: waiting for continue or stop event\n"));

      // wait for continue or stop event
      HANDLE hWaitHandles[2] = { m_hEventContinue, m_hEventStop };
      DWORD dwRet = WaitForMultipleObjects(2, hWaitHandles, FALSE, INFINITE);

      switch (dwRet)
      {
      case WAIT_OBJECT_0: // continue event
         //ATLTRACE(_T("T2: got continue event\n"));
         break;

      case WAIT_OBJECT_0 + 1: // stop event
         //ATLTRACE(_T("T2: got stop event\n"));
         break;

      default:
         // may be timeout or abandoned handle
         //ATLTRACE(_T("T2: error while waiting for continue or stop\n"));
         break;
      }

      // stop loop when event was set
      if (WAIT_OBJECT_0 + 1 == dwRet)
         break;

      // reset continue event
      ResetEvent(m_hEventContinue);

      ////ATLTRACE(_T("T2: resetting wait event\n"));
      //ResetEvent(m_hEventWait);
      ////ATLTRACE(_T("T2: resetting wait event done\n"));

      // must be reset
      ATLASSERT(WAIT_TIMEOUT == ::WaitForSingleObject(m_hEventWait, 0));

      if (!WaitComm())
         break; // error while waiting

      // must still be reset
      ATLASSERT(WAIT_TIMEOUT == ::WaitForSingleObject(m_hEventWait, 0));

      // set event that wait has succeeded
      //ATLTRACE(_T("T2: setting wait event\n"));
      SetEvent(m_hEventWait);
      //ATLTRACE(_T("T2: setting wait event done\n"));

      // must be set
      ATLASSERT(WAIT_OBJECT_0 == ::WaitForSingleObject(m_hEventWait, 0));

      // exit loop when stop event was set meanwhile
   } while (WAIT_TIMEOUT == ::WaitForSingleObject(m_hEventStop, 0));

   //ATLTRACE(_T("T2: stopped worker thread\n"));
   return 0;
}
