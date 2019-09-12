//
// Recorder - a GPS logger app for Windows Mobile
// Copyright (C) 2006-2019 Michael Fink
//
/// \file SerialPort.cpp Serial port implementation
//
#include "stdafx.h"
#include "SerialPort.hpp"

#include "SerialPortImplWinCE.hpp"

using Serial::CSerialPort;

CSerialPort::CSerialPort(LPCTSTR deviceName)
   :m_hPort(NULL),
   m_dwLastError(0),
   m_dwCurrentEventMask(0)
{
   m_hPort = CreateFile( deviceName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);

#ifdef _WIN32_WCE
   if (m_hPort != INVALID_HANDLE_VALUE)
      m_impl.reset(new CSerialPortImpl(m_hPort));
#endif
}

bool CSerialPort::Open()
{
   if (m_impl == NULL)
      return false;

   m_impl->Open();
   return true;
}

void CSerialPort::SetCommState(unsigned int baudrate, unsigned int bits, unsigned int parity, unsigned int stopBits, EHandshakeMode handshakeMode)
{
   DCB dcb = {0};
   GetCommState(m_hPort, &dcb);
   dcb.BaudRate = baudrate;

   dcb.fParity = parity != NOPARITY;
   dcb.Parity = parity;

   dcb.StopBits = stopBits;

   if (handshakeMode == CSerialPort::handshakeOff)
   {
      dcb.fRtsControl = RTS_CONTROL_DISABLE;
      dcb.fDtrControl = DTR_CONTROL_DISABLE;
   }

   ::SetCommState(m_hPort, &dcb);
}

void CSerialPort::SetMask(DWORD mask)
{
   m_dwCurrentEventMask = mask;
   m_impl->SetMask(mask);
}

bool CSerialPort::WaitEvent(DWORD timeout)
{
   DWORD dwEventResult;
   return m_impl->WaitEvent(timeout, m_dwCurrentEventMask, dwEventResult, m_dwLastError);
}

bool CSerialPort::Read(BYTE* buffer, DWORD bufferSize, DWORD& numReadBytes)
{
   return ::ReadFile(m_hPort, buffer, bufferSize, &numReadBytes, NULL) != FALSE;
}

void CSerialPort::Close()
{
   CloseHandle(m_hPort);
   m_hPort = NULL;
   m_impl->Close();
}
