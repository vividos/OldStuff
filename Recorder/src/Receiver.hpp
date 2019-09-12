//
// Recorder - a GPS logger app for Windows Mobile
// Copyright (C) 2006-2019 Michael Fink
//
/// \file Receiver.hpp GPS receiver
//
#pragma once

#include <boost/function.hpp>
#include <ulib/stream/ITextStream.hpp>
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
   void SetRawOutputStream(boost::shared_ptr<Stream::ITextStream> spRawOutputStream =
      boost::shared_ptr<Stream::ITextStream>()) throw()
   {
      ATL::CMutexLock lock(m_mtx);
      m_spRawOutputStream = spRawOutputStream;
   }

private:
   /// called by receiver thread when new raw NMEA0183 data arrives
   void OnNewRawNMEAData(const CString& cszData);

private:
   /// text stream pointer for raw output (e.g. for logging)
   boost::shared_ptr<Stream::ITextStream> m_spRawOutputStream;

   /// serial port for the "serial port" input type
   boost::shared_ptr<Serial::CSerialPort> m_spSerialPort;

   boost::shared_ptr<class CSerialPortReceiverThread> m_spReceiverThread;

   ATL::CMutex m_mtx;

   // test stuff
   T_fnInputCheck m_fnCallback;
};
