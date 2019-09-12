//
// Recorder - a GPS logger app for Windows Mobile
// Copyright (C) 2006-2019 Michael Fink
//
/// \file SerialPort.hpp Serial port
//
#pragma once

namespace Serial
{
   class CSerialPortImpl;

   /// serial port
   class CSerialPort
   {
   public:
      enum EHandshakeMode
      {
         handshakeOff
      };

      CSerialPort(LPCTSTR deviceName);

      bool Open();

      void SetCommState(unsigned int baudrate, unsigned int bits, unsigned int parity, unsigned int stopBits, EHandshakeMode handshakeMode);

      void SetMask(DWORD mask = 0);

      bool WaitEvent(DWORD timeout);

      bool Read(BYTE* buffer, DWORD bufferSize, DWORD& numReadBytes);

      DWORD GetLastError() const
      {
         return m_dwLastError;
      }

      void Close();

   private:
      HANDLE m_hPort;

      DWORD m_dwLastError;

      DWORD m_dwCurrentEventMask;

      boost::shared_ptr<CSerialPortImpl> m_impl;
   };
} // namespace Serial
