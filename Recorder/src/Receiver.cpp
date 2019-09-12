//
// Recorder - a GPS logger app for Windows Mobile
// Copyright (C) 2006-2019 Michael Fink
//
/// \file Receiver.cpp GPS receiver
//
#include "StdAfx.h"
#include "Receiver.hpp"
#include "SerialPort.hpp"
#include "WorkerThread.hpp"
#include <boost/bind.hpp>

#ifdef _WIN32_WCE
#include "BluetoothActivator.hpp"
#endif

class CSerialPortSettings
{
public:
   CSerialPortSettings()
      :m_cszSerialPortDeviceName(_T("com5:")),
      m_uiBaudrate(4800),
      m_uiBits(7),
      m_uiParity(PARITY_EVEN),
      m_uiStopBits(STOPBITS_10),
      m_enHandshakeMode(Serial::CSerialPort::handshakeOff)
   {
   }

   CString m_cszSerialPortDeviceName;

   unsigned int m_uiBaudrate;
   unsigned int m_uiBits;
   unsigned int m_uiParity;
   unsigned int m_uiStopBits;
   Serial::CSerialPort::EHandshakeMode m_enHandshakeMode;
};

class CSerialPortReceiverThread
{
public:
   CSerialPortReceiverThread(boost::function<void(const CString&)> fnOnRawData)
      :m_fnOnRawData(fnOnRawData),
      m_evtStop(TRUE, FALSE),
      m_workerThread(boost::bind(&CSerialPortReceiverThread::Run, this))
   {
   }
   ~CSerialPortReceiverThread()
   {
      Stop();
   }

   void Stop()
   {
      m_evtStop.Set();
      m_workerThread.Join();
   }

   int Run();

private:
   ATL::CEvent m_evtStop;
   boost::function<void(const CString&)> m_fnOnRawData;

   boost::scoped_ptr<Serial::CSerialPort> m_spSerialPort;

   CSerialPortSettings m_settings;

   /// worker thread for receiver
   WorkerThread m_workerThread;

#ifdef _WIN32_WCE
   boost::scoped_ptr<class CBluetoothActivator> m_scpBluetoothActivator;
#endif
};

int CSerialPortReceiverThread::Run()
{
   // open serial port
//   Serial::CSerialPort port(_T("\\device\\com0:"));
//   bool bRet = port.Open();


   // loop while we don't have to stop


   // close serial ports



//   HANDLE hEventStop = receiver.GetStopEventHandle();

   // activate bluetooth when selected, and when current connection is off
#ifdef _WIN32_WCE
   if (m_settings.m_bActivateBluetooth && CBluetoothActivator::GetCurrentMode() == BTH_POWER_OFF)
      m_scpBluetoothActivator.reset(new CBluetoothActivator);
#endif

   m_spSerialPort.reset(new Serial::CSerialPort(m_settings.m_cszSerialPortDeviceName));

   while (WAIT_TIMEOUT == ::WaitForSingleObject(m_evtStop, 0))
   {
      // try to open serial port
      while (!m_spSerialPort->Open() &&
         WAIT_OBJECT_0 != ::WaitForSingleObject(m_evtStop, 0))
      {
         ATLTRACE(_T("T1: couldn't open port, retrying...\n"));
         Sleep(100);
      }

      if (WAIT_OBJECT_0 == ::WaitForSingleObject(m_evtStop, 0))
      {
         ATLTRACE(_T("T1: stop event set, leaving thread, spot 1\n"));

         m_spSerialPort.reset();

         m_evtStop.Reset();
         return 0;
      }

      ATLTRACE(_T("T1: reopened port\n"));

      //receiver.SendEvent(GPS::reConnectReceiver);

      m_spSerialPort->SetCommState(m_settings.m_uiBaudrate, m_settings.m_uiBits,
         m_settings.m_uiParity, m_settings.m_uiStopBits, m_settings.m_enHandshakeMode);

      m_spSerialPort->SetMask();

      while (WAIT_TIMEOUT == ::WaitForSingleObject(m_evtStop, 0))
      {
         DWORD dwT1 = GetTickCount();
         if (true == m_spSerialPort->WaitEvent(1000))
         {
            ATLTRACE(_T("T1: successfully waited for event!\n"));

            BYTE abBuffer[200];
            DWORD dwBufSize = sizeof(abBuffer) / sizeof(*abBuffer);
            abBuffer[dwBufSize - 1] = 0;

            DWORD dwRead = 0;
            m_spSerialPort->Read(abBuffer, dwBufSize - 1, dwRead);
            abBuffer[dwRead] = 0;

            ATLTRACE(_T("T1: sending data upstream\n"));

            m_fnOnRawData(reinterpret_cast<LPCSTR>(abBuffer));
            //receiver.ProcessNMEAData(reinterpret_cast<LPCSTR>(abBuffer));

            ATLTRACE(_T("T1: output:\n%hs\n"), abBuffer);
         }
         else
         {
            DWORD dw = m_spSerialPort->GetLastError();

            DWORD dwT2 = GetTickCount();

            if (dw == ERROR_TIMEOUT)
            {
               ATLTRACE(_T("T1: timeout, time = %u ms\n"), dw, dwT2 - dwT1);
               dwT1 = GetTickCount();
               continue; // try to continue waiting for events
            }

            ATLTRACE(_T("T1: wait not successful, error = %08x, time = %u ms\n"), dw, dwT2 - dwT1);

            ATLTRACE(_T("T1: closing port\n"));
            dwT1 = GetTickCount();
            m_spSerialPort->SetMask(0);
            m_spSerialPort->Close();
            dwT2 = GetTickCount();
            ATLTRACE(_T("T1: closing port done, took %u ms\n"), dwT2 - dwT1);

            //receiver.SendEvent(GPS::reDisconnectReceiver);

            break;
         }

         ATLTRACE(_T("T1: re-checking event!\n"));
      }
   }

   ATLTRACE(_T("T1: stop event set, leaving thread, spot 2\n"));

   m_spSerialPort->Close();

   m_evtStop.Reset();

   m_spSerialPort.reset();

   //   receiver.SendEvent(GPS::reDisconnectReceiver);

   return 0;
}

CReceiver::CReceiver()
   :m_mtx(FALSE)
{
}

CReceiver::~CReceiver()
{
   ATL::CMutexLock lock(m_mtx);
   if (m_spReceiverThread != NULL)
      Stop();
}

void CReceiver::Start()
{
   ATL::CMutexLock lock(m_mtx);

   m_spReceiverThread.reset(new CSerialPortReceiverThread(
      boost::bind(&CReceiver::OnNewRawNMEAData, this, _1)));
}

void CReceiver::Stop()
{
   boost::shared_ptr<CSerialPortReceiverThread> spReceiverThread;
   {
      ATL::CMutexLock lock(m_mtx);
      spReceiverThread = m_spReceiverThread;
      m_spReceiverThread.reset();
      m_fnCallback.clear();
   }

   spReceiverThread->Stop();
   spReceiverThread.reset();
}

void CReceiver::OnNewRawNMEAData(const CString& cszData)
{
   ATL::CMutexLock lock(m_mtx);

   if (m_spRawOutputStream != NULL)
   {
      Stream::ITextStream& stream = *m_spRawOutputStream;
      ATLASSERT(stream.CanWrite());
      stream.Write(cszData);
   }

   if (m_fnCallback != NULL)
      m_fnCallback(cszData);
}
