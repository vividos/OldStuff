//
// Recorder - a GPS logger app for Windows Mobile
// Copyright (C) 2006-2019 Michael Fink
//
/// \file BatteryManager.hpp Battery manager
//
#pragma once

#include <vector>

#ifdef _WIN32_WCE
class SystemPowerStatusEx : public SYSTEM_POWER_STATUS_EX
{
public:
   SystemPowerStatusEx(bool bUseCachedValues = false) throw();
};
#endif

class CBatteryManager
{
public:
   CBatteryManager() throw()
      :m_bLastBatteryLifePercent(BATTERY_PERCENTAGE_UNKNOWN)
   {
   }

   /// returns current battery capacity
   static BYTE BatteryCapacity() throw();

   /// call this from time to time (e.g. each minute) to calculate remaining time
   void Tick() throw();

   /// returns estimated remaining time
   COleDateTimeSpan RemainingTime() const throw();

private:
   std::vector<std::pair<COleDateTime, BYTE> > m_vecTimeAndBatteryStatus;
   BYTE m_bLastBatteryLifePercent;
};
