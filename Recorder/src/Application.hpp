//
// Recorder - a GPS logger app for Windows Mobile
// Copyright (C) 2006-2019 Michael Fink
//
/// \file Application.hpp Application object
//
#pragma once

#include "Settings.hpp"
#include "Logger.hpp"
#include "Receiver.hpp"
#include "BatteryManager.hpp"
#include "DisplayOffManager.hpp"


namespace Logging
{
   static LPCTSTR App = _T("app");
}


class CApplication
{
public:
   CApplication() throw();
   ~CApplication() throw();

   void AppendNMEALog(const CString& cszFilename);
   void CloseNMEALog();

   CSettings& Settings() throw() { return m_settings; }

   CBatteryManager& BatteryManager() throw() { return m_batteryManager; }

   CDisplayOffManager& DisplayOffManager() throw() { return m_displayOffManager; }

   CReceiver& Receiver() throw() { return m_receiver; }

private:
   CSettings m_settings;
   CBatteryManager m_batteryManager;
   CDisplayOffManager m_displayOffManager;

   CReceiver m_receiver;

   boost::shared_ptr<Stream::ITextStream> m_spNMEALogTextStream;
};
