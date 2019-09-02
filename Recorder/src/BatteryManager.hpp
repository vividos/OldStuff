#pragma once

#include <ulib/DateTime.hpp>
#include <vector>

class SystemPowerStatusEx: public SYSTEM_POWER_STATUS_EX
{
public:
   SystemPowerStatusEx(bool bUseCachedValues = false) throw();
};

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
   TimeSpan RemainingTime() const throw();

private:
   std::vector<std::pair<DateTime, BYTE> > m_vecTimeAndBatteryStatus;
   BYTE m_bLastBatteryLifePercent;
};
