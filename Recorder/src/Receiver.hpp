//
// Recorder - a GPS logger app for Windows Mobile
// Copyright (C) 2006-2019 Michael Fink
//
/// \file Receiver.hpp GPS receiver
//
#pragma once

#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>

namespace Serial
{
   class CSerialPort;
}

typedef boost::function<void(const CString&)> T_fnInputCheck;

/// GPS receiver
class CReceiver
{
public:
   /// ctor
   CReceiver();
   ~CReceiver();

   /// starts the receiver thread
   void Start();
   void Stop();

   void SetInputCheckCallback(T_fnInputCheck fnCallback = T_fnInputCheck())
   {
      ATL::CMutexLock lock(m_mtx);
      m_fnCallback = fnCallback;
   }

   /// sets text stream pointer for raw nmea output
   void SetRawOutputStream(FILE* outputStream = NULL)
   {
      ATL::CMutexLock lock(m_mtx);
      m_outputStream = outputStream;
   }

private:
   /// called by receiver thread when new raw NMEA0183 data arrives
   void OnNewRawNMEAData(const CString& cszData);

private:
   /// file for raw output (e.g. for logging)
   FILE* m_outputStream;

   /// serial port for the "serial port" input type
   boost::shared_ptr<Serial::CSerialPort> m_spSerialPort;

   boost::shared_ptr<class CSerialPortReceiverThread> m_spReceiverThread;

   ATL::CMutex m_mtx;

   // test stuff
   T_fnInputCheck m_fnCallback;
};
