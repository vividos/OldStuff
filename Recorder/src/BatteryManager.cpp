#include "StdAfx.h"
#include "BatteryManager.hpp"
//#include <ulib/log/Logger.hpp>
#include <ulib/win32/ErrorMessage.hpp>

SystemPowerStatusEx::SystemPowerStatusEx(bool bUseCachedValues) throw()
{
   ZeroMemory(this, sizeof(*this));

   BOOL bRet = GetSystemPowerStatusEx(this, bUseCachedValues ? FALSE : TRUE);
   if (bRet == FALSE)
      ATLTRACE(_T("GetSystemPowerStatusEx failed: ") +
         Win32::ErrorMessage().Get());
}

BYTE CBatteryManager::BatteryCapacity() throw()
{
   return SystemPowerStatusEx().BatteryLifePercent;
}

void CBatteryManager::Tick() throw()
{
   SystemPowerStatusEx systemPowerStatus;

   // check battery flag
   if ((systemPowerStatus.BatteryFlag & BATTERY_FLAG_CHARGING) != 0)
   {
      // device is loading, so clear previous valuess
      m_vecTimeAndBatteryStatus.clear();
      return;
   }

   BYTE bPercentage = systemPowerStatus.BatteryLifePercent;

   // we can't do anything when percentage is unknown
   if (bPercentage == BATTERY_PERCENTAGE_UNKNOWN)
      return;

   // we had previous values?
   if (m_bLastBatteryLifePercent != BATTERY_PERCENTAGE_UNKNOWN)
   {
      // percentage didn't change in between?
      if (bPercentage == m_bLastBatteryLifePercent)
         return;

      // if last value was smaller than this one, the device was charged in
      // between. ignore all collected values, but record new one
      if (bPercentage > m_bLastBatteryLifePercent)
      {
         m_vecTimeAndBatteryStatus.clear();
      }
   }

   // now we're at the stage where the new value can be recorded

   // record a new point
   m_vecTimeAndBatteryStatus.push_back(std::make_pair(DateTime::Now(), bPercentage));
   m_bLastBatteryLifePercent = bPercentage;

   // truncate list; only use last 50 points
   if (m_vecTimeAndBatteryStatus.size() > 50)
   {
      m_vecTimeAndBatteryStatus.erase(m_vecTimeAndBatteryStatus.begin(),
         m_vecTimeAndBatteryStatus.begin()+m_vecTimeAndBatteryStatus.size()-50);
   }
}

TimeSpan CBatteryManager::RemainingTime() const throw()
{
   // when empty or only one value, we can't do calculations
   if (m_vecTimeAndBatteryStatus.empty() || m_vecTimeAndBatteryStatus.size() == 1)
      return TimeSpan(0, 0, 0, 0);

   // m_vecTimeAndBatteryStatus should contain only monotonically decreasing percentage
   // values, so we can calculate the (timespan / percentage) ratio for all values.
   // then we calculate the average and derive how long the device lasts until it is at 0%

   double dRatioAverage = 0.0;

   {
      CString cszText;
      cszText.Format(_T("RemainingTime() called, %u values in vector"),
         m_vecTimeAndBatteryStatus.size());

//      LOG_DEBUG(cszText, _T("app.batterymanager"));
   }

   for (size_t i=0,iMax=m_vecTimeAndBatteryStatus.size()-1; i<iMax; i++)
   {
      const std::pair<DateTime, BYTE>& status1 = m_vecTimeAndBatteryStatus[i],
         status2 = m_vecTimeAndBatteryStatus[i+1];

      if (status1.first > status2.first || status1.second < status2.second)
         continue;

      BYTE bPercentDiff = static_cast<BYTE>(status1.second - status2.second);
      if (bPercentDiff == 0)
         continue;

      TimeSpan tsTimeDiff = status2.first - status1.first;

      double dRatio = tsTimeDiff.TotalSeconds() / double(bPercentDiff);

      {
         CString cszText;
         cszText.Format(_T("%u: dp=%u%%, dt=%us, ratio=%u.%03u"),
            i, bPercentDiff,
            static_cast<unsigned int>(tsTimeDiff.TotalSeconds()),
            static_cast<unsigned int>(dRatio),
            static_cast<unsigned int>(dRatio-static_cast<int>(dRatio)*1000.0));

//         LOG_DEBUG(cszText, _T("app.batterymanager"));
      }

      dRatioAverage += dRatio;
   }

   // calculate average
   dRatioAverage /= m_vecTimeAndBatteryStatus.size()-1;

   {
      CString cszText;
      cszText.Format(_T("calculated time/percent=%u s, still left: %u min"),
         static_cast<unsigned int>(dRatioAverage),
         static_cast<unsigned int>(m_bLastBatteryLifePercent * dRatioAverage / 60.0));

//      LOG_DEBUG(cszText, _T("app.batterymanager"));
   }

   // now we have the number of seconds for a decrease of one percent
   // calculate remaining time to 0%
   return TimeSpan(0, 0, static_cast<int>(m_bLastBatteryLifePercent * dRatioAverage), 0);
}
