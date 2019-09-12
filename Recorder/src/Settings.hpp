//
// Recorder - a GPS logger app for Windows Mobile
// Copyright (C) 2006-2019 Michael Fink
//
/// \file Settings.hpp Settings class
//
#pragma once

#include <stdexcept>

namespace Settings
{
   static const TCHAR* TimerBatteryManager = _T("Settings.BatteryManager.Timer");

   static const TCHAR* ReceiverSerialPort = _T("Settings.Receiver.SerialPort");
   static const TCHAR* ReceiverBaudrate = _T("Settings.Receiver.Baudrate");
   static const TCHAR* ReceiverActivateBluetooth = _T("Settings.Receiver.BluetoothActivate");
}

class CSettings
{
public:
   CSettings() throw() {}

   bool GetBoolean(const CString& cszName)
   {
      if (cszName == Settings::ReceiverActivateBluetooth)
      {
         return true;
      }

      ThrowUnknownSettingsException(cszName);
   }

   CString GetString(const CString& cszName)
   {
      if (cszName == Settings::ReceiverSerialPort)
      {
         return _T("COM4:");
      }

      ThrowUnknownSettingsException(cszName);
   }

   unsigned int GetUInt(const CString& cszName)
   {
      if (cszName == Settings::TimerBatteryManager)
      {
         // every 60 seconds
         return 60;
      }
      else if (cszName == Settings::ReceiverBaudrate)
      {
         return 38400;
      }

      ThrowUnknownSettingsException(cszName);
   }

private:
   __declspec(noreturn) void ThrowUnknownSettingsException(const CString& cszName)
   {
      std::string msg("unknown settings name: ");
      msg += CStringA(cszName);
      throw std::runtime_error(msg);
   }
};
