//
// ulib - a collection of useful classes
// Copyright (C) 2006-2014 Michael Fink
//
/// \file DateTime.cpp date/time class
//

// includes
#include "stdafx.h"
#include <ulib/DateTime.hpp>
#include <ulib/Exception.hpp>
#include "ISO8601Parser.hpp"

// include Boost.DateTime
#define BOOST_DATE_TIME_NO_LIB

#pragma warning(push)
#pragma warning(disable: 4244) // 'argument' : conversion from 'T1' to 'T2', possible loss of data
#pragma warning(disable: 4245) // 'initializing' : conversion from 'T1' to 'T2', signed/unsigned mismatch
#include <boost/date_time.hpp>
#pragma warning(pop)

/// registry path with time zone infos
LPCTSTR g_pszRegTimeZonesPath = _T("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Time Zones");

TimeZone::TimeZone()
{
   ZeroMemory(&m_tzi, sizeof(m_tzi));
}

TimeZone TimeZone::UTC() throw()
{
   TimeZone tz;
   _tcsncpy_s(tz.m_tzi.StandardName, sizeof(tz.m_tzi.StandardName)/sizeof(TCHAR),
      _T("UTC"), sizeof(tz.m_tzi.StandardName)/sizeof(TCHAR));
   return tz;
}

TimeZone TimeZone::System()
{
   TimeZone tz;
   DWORD dwRet = GetTimeZoneInformation(&tz.m_tzi);

   if (TIME_ZONE_ID_INVALID == dwRet)
      throw Exception(_T("couldn't get system timezone info"), __FILE__, __LINE__);

   return tz;
}

#pragma pack(push)
#pragma pack(1)

/// registry timezone info format
struct REG_TZI_FORMAT
{
   long Bias;              ///< bias in minutes
   long StandardBias;      ///< bias in minutes, during standard time
   long DaylightBias;      ///< bias in minutes, during daylight savings time
   SYSTEMTIME StandardDate;///< date and local time for transistion from daylight savings time to standard time
   SYSTEMTIME DaylightDate;///< date and local time for transistion from standard time to daylight savings time
};

#pragma pack(pop)

std::vector<TimeZone> TimeZone::EnumerateTimezones()
{
   CRegKey regKeyTimeZones;
   if (ERROR_SUCCESS != regKeyTimeZones.Open(HKEY_LOCAL_MACHINE, g_pszRegTimeZonesPath,
         KEY_ENUMERATE_SUB_KEYS))
      throw Exception(_T("couldn't enumerate timezones"), __FILE__, __LINE__);

   std::vector<TimeZone> vecTimezones;

   DWORD dwSize = 256;
   TCHAR szBuffer[256];
   DWORD dwIndex = 0;
   while (ERROR_SUCCESS == ::RegEnumKeyEx(regKeyTimeZones.m_hKey, dwIndex++, szBuffer, &dwSize, NULL, NULL, NULL, NULL))
   {
      CRegKey regKeyTimeZoneInfo;
      if (ERROR_SUCCESS != regKeyTimeZoneInfo.Open(regKeyTimeZones,
         szBuffer, KEY_QUERY_VALUE))
        throw Exception(_T("couldn't enumerate timezones"), __FILE__, __LINE__);

      TimeZone timeZone;

      // read time zone names
      ULONG ulCount = sizeof(timeZone.m_tzi.StandardName) / sizeof(TCHAR) - 1;
      regKeyTimeZoneInfo.QueryStringValue(_T("Std"), timeZone.m_tzi.StandardName, &ulCount);

      ulCount = sizeof(timeZone.m_tzi.DaylightName) / sizeof(TCHAR) - 1;
      regKeyTimeZoneInfo.QueryStringValue(_T("Dlt"), timeZone.m_tzi.DaylightName, &ulCount);

      ulCount = 256;
      regKeyTimeZoneInfo.QueryStringValue(_T("Display"), timeZone.m_cszDisplayName.GetBuffer(ulCount), &ulCount);
      timeZone.m_cszDisplayName.ReleaseBuffer(ulCount / sizeof(TCHAR));

      // get type and length
      DWORD dwType = 0, dwLength = 0;
      if (ERROR_SUCCESS != RegQueryValueEx(regKeyTimeZoneInfo.m_hKey, _T("TZI"), NULL, &dwType, NULL, &dwLength) ||
          dwLength != 44 || dwType != REG_BINARY)
         throw Exception(_T("couldn't enumerate timezones"), __FILE__, __LINE__);

      REG_TZI_FORMAT tzi;

      // get binary data
      static_assert(sizeof(REG_TZI_FORMAT) == 44, "struct REG_TZI_FORMAT must have a size of 44");

      if (ERROR_SUCCESS != RegQueryValueEx(regKeyTimeZoneInfo.m_hKey, _T("TZI"), NULL, NULL, reinterpret_cast<LPBYTE>(&tzi), &dwLength))
         throw Exception(_T("couldn't enumerate timezones"), __FILE__, __LINE__);

      timeZone.m_tzi.Bias = tzi.Bias;
      timeZone.m_tzi.StandardBias = tzi.StandardBias;
      timeZone.m_tzi.DaylightBias = tzi.DaylightBias;
      timeZone.m_tzi.StandardDate = tzi.StandardDate;
      timeZone.m_tzi.DaylightDate = tzi.DaylightDate;

      vecTimezones.push_back(timeZone);

      dwSize = 256; // prepare for next call to RegEnumKeyEx
   }

   std::sort(vecTimezones.begin(), vecTimezones.end(), CompareGreaterBias);

   return vecTimezones;
}

TimeSpan TimeZone::GetUtcOffset(const DateTime& dt) const
{
   if (!IsDaylightSavingTime(dt))
      return TimeSpan(0, -(m_tzi.Bias + m_tzi.StandardBias), 0, 0);
   else
      return TimeSpan(0, -(m_tzi.Bias + m_tzi.DaylightBias), 0, 0);
}

/// converts SYSTEMTIME to DateTime
DateTime SystemTimeToDateTime(const SYSTEMTIME& st)
{
   return DateTime(
      st.wYear,
      st.wMonth,
      st.wDay,
      st.wHour,
      st.wMinute,
      st.wSecond,
      st.wMilliseconds);
}

/// from:
/// http://www.ptgsystems.com/blognet/post/2010/03/12/determining-the-nth-occurence-of-a-day-of-the-week-in-any-given-month.aspx
DateTime TimeZone::CalculateTransitionDate(unsigned int uiYear, unsigned int uiIndex) const
{
   ATLASSERT(uiIndex == 0 || uiIndex == 1);

   const SYSTEMTIME& st = uiIndex == 0 ? m_tzi.DaylightDate : m_tzi.StandardDate;
   SYSTEMTIME stTemp = st;

   if (stTemp.wYear != 0 ||                           // one-time switch
       ((stTemp.wDay  < 1)  || (stTemp.wDay > 5)) ||  // invalid number of week
       stTemp.wDayOfWeek > 6)                         // invalid day of week
   {
      ATLASSERT(false);
      return DateTime(DateTime::invalid);
   }

   stTemp.wYear = static_cast<WORD>(uiYear); // set year

   stTemp.wDay = 1;
   FILETIME ft;
   SystemTimeToFileTime(&stTemp, &ft);
   FileTimeToSystemTime(&ft, &stTemp);

   int nDayOff;

   // determine offset to first target WeekDay
   nDayOff = (int)st.wDayOfWeek - (int)stTemp.wDayOfWeek;
   if (nDayOff < 0)
      nDayOff += 7;

   // offset to wDay occurence of WeekDay
   nDayOff += ((st.wDay-1) * 7);
   stTemp.wDay = static_cast<WORD>(nDayOff + 1);
   BOOL bValid = SystemTimeToFileTime(&stTemp, &ft);
   if (bValid)
   {
      SYSTEMTIME stFinal = {0};
      FileTimeToSystemTime(&ft, &stFinal);

      return SystemTimeToDateTime(stFinal);
   }
   else if (st.wDay == 5)    // Only allow backup of 1 week if wDay = 5 (last occurrence in month)
   {
      nDayOff -= 7;
      stTemp.wDay = static_cast<WORD>(nDayOff + 1);
      bValid = SystemTimeToFileTime(&stTemp, &ft);

      SYSTEMTIME stFinal = {0};
      FileTimeToSystemTime(&ft, &stFinal);

      if (bValid)
         return SystemTimeToDateTime(stFinal);
   }

   return DateTime(DateTime::invalid);
}

bool TimeZone::IsDaylightSavingTime(const DateTime& dt) const
{
   // check if there are valid transition dates at all
   if (m_tzi.StandardDate.wMonth == 0 ||
       m_tzi.DaylightDate.wMonth == 0)
      return false; // no daylight savings time without transition dates

   // convert transition dates to absolute dates
   DateTime dtTrans1 = CalculateTransitionDate(dt.Year(), 0);
   DateTime dtTrans2 = CalculateTransitionDate(dt.Year(), 1);

   if (dtTrans1.Status() != DateTime::valid &&
       dtTrans2.Status() != DateTime::valid)
      return false; // no daylight savings time

   //ATLTRACE(_T("t1=%s\nt2=%s\ndt=%s"),
   //   dtTrans1.Format(_T("%Y-%m-%dT%H:%M:%S")),
   //   dtTrans2.Format(_T("%Y-%m-%dT%H:%M:%S")),
   //   dt.Format(_T("%Y-%m-%dT%H:%M:%S")));

   // compare if given date/time is in the range
   return dtTrans1 < dt && dt < dtTrans2;
}

bool TimeZone::CompareLessBias(const TimeZone& lhs, const TimeZone& rhs)
{
   return lhs.m_tzi.Bias < rhs.m_tzi.Bias;
}

bool TimeZone::CompareGreaterBias(const TimeZone& lhs, const TimeZone& rhs)
{
   return lhs.m_tzi.Bias > rhs.m_tzi.Bias;
}

/// \brief time span implementation
/// \details uses boost::date_time classes
class TimeSpanImpl
{
public:
   /// ctor; initializes using special value
   TimeSpanImpl(boost::date_time::special_values sv = boost::date_time::not_a_date_time)
      :m_span(sv)
   {
   }

   /// ctor; initializes using given time_duration object
   TimeSpanImpl(const boost::posix_time::time_duration& span)
      :m_span(span)
   {
   }

   /// time duration object
   boost::posix_time::time_duration m_span;
};


TimeSpan::TimeSpan() throw()
:m_spImpl(new TimeSpanImpl)
{
}

TimeSpan::TimeSpan(int iHours, int iMins, int iSecs, int iMillisecs) throw()
:m_spImpl(new TimeSpanImpl)
{
   SetDateTimeSpan(iHours, iMins, iSecs, iMillisecs);
}

TimeSpan::TimeSpan(T_enStatus status)
:m_spImpl(new TimeSpanImpl)
{
   ATLASSERT(status == TimeSpan::min || status == TimeSpan::max);

   switch(status)
   {
   case TimeSpan::min: m_spImpl->m_span = boost::date_time::min_date_time; break;
   case TimeSpan::max: m_spImpl->m_span = boost::date_time::max_date_time; break;
   default:
      throw Exception(_T("invalid status in ctor"), __FILE__, __LINE__);
   }
}

TimeSpan::TimeSpan(const TimeSpan& dt)
:m_spImpl(dt.m_spImpl)
{
}

TimeSpan& TimeSpan::operator=(const TimeSpan& rhs)
{
   m_spImpl = rhs.m_spImpl;
   return *this;
}

TimeSpan::T_enStatus TimeSpan::Status() const throw()
{
   ATLASSERT(m_spImpl != NULL);

   if (m_spImpl->m_span.is_not_a_date_time())
      return TimeSpan::invalid;
   else if (m_spImpl->m_span == boost::date_time::min_date_time)
      return TimeSpan::min;
   else if (m_spImpl->m_span == boost::date_time::max_date_time)
      return TimeSpan::max;
   else
      return TimeSpan::valid;
}

int TimeSpan::Hours() const throw()
{
   ATLASSERT(m_spImpl != NULL);
   return m_spImpl->m_span.hours();
}

int TimeSpan::Minutes() const throw()
{
   ATLASSERT(m_spImpl != NULL);
   return m_spImpl->m_span.minutes();
}

int TimeSpan::Seconds() const throw()
{
   ATLASSERT(m_spImpl != NULL);
   return m_spImpl->m_span.seconds();
}

int TimeSpan::Milliseconds() const throw()
{
   ATLASSERT(m_spImpl != NULL);
   return static_cast<int>(m_spImpl->m_span.fractional_seconds()) / 1000;
}

double TimeSpan::TotalHours() const throw()
{
   ATLASSERT(m_spImpl != NULL);
   return static_cast<double>(m_spImpl->m_span.ticks() / (3600*m_spImpl->m_span.ticks_per_second()));
}

double TimeSpan::TotalMinutes() const throw()
{
   ATLASSERT(m_spImpl != NULL);
   return static_cast<double>(m_spImpl->m_span.ticks() / (60*m_spImpl->m_span.ticks_per_second()));
}

double TimeSpan::TotalSeconds() const throw()
{
   ATLASSERT(m_spImpl != NULL);
   return static_cast<double>(m_spImpl->m_span.ticks() / (m_spImpl->m_span.ticks_per_second()));
}

double TimeSpan::TotalMilliseconds() const throw()
{
   ATLASSERT(m_spImpl != NULL);
   return static_cast<double>(m_spImpl->m_span.ticks() / (m_spImpl->m_span.ticks_per_second()/1000.0));
}

TimeSpan TimeSpan::operator+(const TimeSpan& rhs) const throw()
{
   ATLASSERT(m_spImpl != NULL);
   ATLASSERT(rhs.m_spImpl != NULL);

   TimeSpan dts;
   dts.m_spImpl->m_span = m_spImpl->m_span + rhs.m_spImpl->m_span;
   return dts;
}

TimeSpan TimeSpan::operator-(const TimeSpan& rhs) const throw()
{
   ATLASSERT(m_spImpl != NULL);
   ATLASSERT(rhs.m_spImpl != NULL);

   TimeSpan dts;
   dts.m_spImpl->m_span = m_spImpl->m_span - rhs.m_spImpl->m_span;
   return dts;
}

TimeSpan TimeSpan::operator*(int iFactor) const throw()
{
   ATLASSERT(m_spImpl != NULL);

   TimeSpan dts;
   dts.m_spImpl->m_span = m_spImpl->m_span * iFactor;
   return dts;
}

TimeSpan TimeSpan::operator/(int iFactor) const throw()
{
   ATLASSERT(m_spImpl != NULL);

   TimeSpan dts;
   dts.m_spImpl->m_span = m_spImpl->m_span / iFactor;
   return dts;
}

TimeSpan& TimeSpan::operator+=(const TimeSpan span) throw()
{
   ATLASSERT(m_spImpl != NULL);
   ATLASSERT(span.m_spImpl != NULL);

   PrepareCopy();

   m_spImpl->m_span += span.m_spImpl->m_span;
   return *this;
}

TimeSpan& TimeSpan::operator-=(const TimeSpan span) throw()
{
   ATLASSERT(m_spImpl != NULL);
   ATLASSERT(span.m_spImpl != NULL);

   PrepareCopy();

   m_spImpl->m_span -= span.m_spImpl->m_span;
   return *this;
}

TimeSpan& TimeSpan::operator*=(int iFactor) throw()
{
   ATLASSERT(m_spImpl != NULL);

   PrepareCopy();

   m_spImpl->m_span *= iFactor;
   return *this;
}

TimeSpan& TimeSpan::operator/=(int iFactor) throw()
{
   ATLASSERT(m_spImpl != NULL);

   PrepareCopy();

   m_spImpl->m_span /= iFactor;
   return *this;
}

TimeSpan TimeSpan::operator-() const throw()
{
   ATLASSERT(m_spImpl != NULL);

   TimeSpan dts(*this);
   dts.PrepareCopy();
   dts.m_spImpl->m_span = -dts.m_spImpl->m_span;

   return dts;
}

bool TimeSpan::operator==(const TimeSpan& rhs) const throw()
{
   ATLASSERT(m_spImpl != NULL);
   ATLASSERT(rhs.m_spImpl != NULL);

   return m_spImpl->m_span == rhs.m_spImpl->m_span;
}

bool TimeSpan::operator!=(const TimeSpan& rhs) const throw()
{
   ATLASSERT(m_spImpl != NULL);
   ATLASSERT(rhs.m_spImpl != NULL);

   return m_spImpl->m_span != rhs.m_spImpl->m_span;
}

bool TimeSpan::operator>(const TimeSpan& rhs) const throw()
{
   ATLASSERT(m_spImpl != NULL);
   ATLASSERT(rhs.m_spImpl != NULL);

   return m_spImpl->m_span > rhs.m_spImpl->m_span;
}

bool TimeSpan::operator<(const TimeSpan& rhs) const throw()
{
   ATLASSERT(m_spImpl != NULL);
   ATLASSERT(rhs.m_spImpl != NULL);

   return m_spImpl->m_span < rhs.m_spImpl->m_span;
}

void TimeSpan::SetDateTimeSpan(int iHours, int iMins, int iSecs, int iMillisecs) throw()
{
   ATLASSERT(m_spImpl != NULL);

   PrepareCopy();

   try
   {
      // note: multiplying milliseconds with 1000 here, since resolution is in microsecs
      m_spImpl->m_span = boost::posix_time::time_duration(iHours, iMins, iSecs, iMillisecs*1000);
   }
   catch(...)
   {
      // illegal date/time span value
      m_spImpl->m_span = boost::date_time::not_a_date_time;
   }
}

CString TimeSpan::FormatISO8601() const throw()
{
   ATLASSERT(m_spImpl != NULL);

   if (m_spImpl->m_span.is_not_a_date_time())
      return _T("");

   CString cszFormat(_T("P%H:%M:%S"));

   struct tm tmTemp = boost::posix_time::to_tm(m_spImpl->m_span);

   CString cszDateTime;
   LPTSTR pszBuffer = cszDateTime.GetBufferSetLength(256);
   _tcsftime(pszBuffer, cszDateTime.GetLength(), cszFormat, &tmTemp);
   cszDateTime.ReleaseBuffer();

   return cszDateTime;
}

void TimeSpan::PrepareCopy() throw()
{
   ATLASSERT(m_spImpl != NULL);

   if (!m_spImpl.unique())
      m_spImpl.reset(new TimeSpanImpl(m_spImpl->m_span));
}

// ///////////////////////////////////

/// \brief DateTime implementation
/// \details uses boost::date_time classes
class DateTimeImpl
{
public:
   /// ctor; initializes using special value
   DateTimeImpl(boost::date_time::special_values sv = boost::date_time::not_a_date_time)
      :m_dt(sv)
   {
   }

   /// ctor; initializes using existing ptime object
   DateTimeImpl(const boost::posix_time::ptime& dt)
      :m_dt(dt)
   {
   }

   /// posix time object
   boost::posix_time::ptime m_dt;
};

DateTime::DateTime()
:m_spImpl(new DateTimeImpl)
{
}

DateTime::DateTime(unsigned int uiYear, unsigned int uiMonth, unsigned int uiDay,
   unsigned int uiHour, unsigned int uiMinute, unsigned int uiSecond, unsigned int uiMillisecond)
:m_spImpl(new DateTimeImpl)
{
   try
   {
      m_spImpl->m_dt = boost::posix_time::ptime(
         boost::gregorian::date(USHORT(uiYear), USHORT(uiMonth), USHORT(uiDay)),
         boost::posix_time::time_duration(uiHour, uiMinute, uiSecond, uiMillisecond*1000));
   }
   catch(...)
   {
      // illegal date/time value(s)
      m_spImpl->m_dt = boost::date_time::not_a_date_time;
   }
}

DateTime::DateTime(T_enStatus status)
:m_spImpl(new DateTimeImpl)
{
   ATLASSERT(status == DateTime::min || status == DateTime::max);

   switch(status)
   {
   case DateTime::min: m_spImpl->m_dt = boost::date_time::min_date_time; break;
   case DateTime::max: m_spImpl->m_dt = boost::date_time::max_date_time; break;
   default:
      throw Exception(_T("invalid status in ctor"), __FILE__, __LINE__);
   }
}

DateTime::DateTime(const DateTime& dt)
:m_spImpl(dt.m_spImpl) // copy on write
{
}

DateTime& DateTime::operator=(const DateTime& rhs)
{
   m_spImpl = rhs.m_spImpl; // copy on write
   return *this;
}

DateTime DateTime::Now() throw()
{
/*#ifndef _WIN32_WCE
   time_t now = time(NULL);
   struct tm tmNow = *gmtime(&now);

   return DateTime(
      tmNow.tm_year+1900,  tmNow.tm_mon+1,   tmNow.tm_mday,
      tmNow.tm_hour,       tmNow.tm_min,     tmNow.tm_sec);
#else
*/
   SYSTEMTIME stNow;
   GetSystemTime(&stNow);
   return DateTime(stNow.wYear, stNow.wMonth, stNow.wDay,
      stNow.wHour, stNow.wMinute, stNow.wSecond, stNow.wMilliseconds);
//#endif
}

DateTime DateTime::Today() throw()
{
   DateTime dt = DateTime::Now();

   dt.m_spImpl->m_dt = boost::posix_time::ptime(dt.m_spImpl->m_dt.date());

   return dt;
}


void DateTime::SetDateTime(unsigned int uiYear, unsigned int uiMonth, unsigned int uiDay,
   unsigned int uiHour, unsigned int uiMinute, unsigned int uiSecond, unsigned int uiMillisecond) throw()
{
   PrepareCopy();

   try
   {
      m_spImpl->m_dt = boost::posix_time::ptime(
         boost::gregorian::date(USHORT(uiYear), USHORT(uiMonth), USHORT(uiDay)),
         boost::posix_time::time_duration(uiHour, uiMinute, uiSecond, uiMillisecond*1000));
   }
   catch(...)
   {
      // illegal date/time span value
      m_spImpl->m_dt = boost::date_time::not_a_date_time;
   }
}

DateTime::T_enStatus DateTime::Status() const throw()
{
   ATLASSERT(m_spImpl != NULL);

   if (m_spImpl->m_dt.is_not_a_date_time())
      return DateTime::invalid;
   else if (m_spImpl->m_dt == boost::date_time::min_date_time)
      return DateTime::min;
   else if (m_spImpl->m_dt == boost::date_time::max_date_time)
      return DateTime::max;
   else
      return DateTime::valid;
}

unsigned int DateTime::Year() const throw()
{
   ATLASSERT(m_spImpl != NULL);
   return m_spImpl->m_dt.date().year();
}

unsigned int DateTime::Month() const throw()
{
   ATLASSERT(m_spImpl != NULL);
   return m_spImpl->m_dt.date().month();
}

unsigned int DateTime::Day() const throw()
{
   ATLASSERT(m_spImpl != NULL);
   return m_spImpl->m_dt.date().day();
}

unsigned int DateTime::Hour() const throw()
{
   ATLASSERT(m_spImpl != NULL);
   return m_spImpl->m_dt.time_of_day().hours();
}

unsigned int DateTime::Minute() const throw()
{
   ATLASSERT(m_spImpl != NULL);
   return m_spImpl->m_dt.time_of_day().minutes();
}

unsigned int DateTime::Second() const throw()
{
   ATLASSERT(m_spImpl != NULL);
   return m_spImpl->m_dt.time_of_day().seconds();
}

unsigned int DateTime::Millisecond() const throw()
{
   ATLASSERT(m_spImpl != NULL);
   return static_cast<unsigned int>(m_spImpl->m_dt.time_of_day().fractional_seconds()) / 1000;
}

unsigned int DateTime::DayOfWeek() const throw()
{
   ATLASSERT(m_spImpl != NULL);
   // day of week returns numbers from 0..6, 0 being sunday
   return m_spImpl->m_dt.date().day_of_week().as_number();
}

unsigned int DateTime::DayOfYear() const throw()
{
   ATLASSERT(m_spImpl != NULL);
   // day of year returns numbers from 1..366
   return m_spImpl->m_dt.date().day_of_year();
}

TimeSpan DateTime::TimeOfDay() throw()
{
   return TimeSpan(Hour(), Minute(), Second(), Millisecond());
}

// operators

DateTime DateTime::operator+(const TimeSpan& rhs) const throw()
{
   DateTime dt(*this);
   dt += rhs;
   return dt;
}

TimeSpan DateTime::operator-(const DateTime& rhs) const throw()
{
   ATLASSERT(m_spImpl != NULL);

   TimeSpan span;
   span.m_spImpl->m_span = m_spImpl->m_dt - rhs.m_spImpl->m_dt;
   return span;
}

DateTime& DateTime::operator+=(const TimeSpan& span) throw()
{
   ATLASSERT(m_spImpl != NULL);
   PrepareCopy();
   m_spImpl->m_dt += span.m_spImpl->m_span;
   return *this;
}

DateTime& DateTime::operator-=(const TimeSpan& span) throw()
{
   ATLASSERT(m_spImpl != NULL);
   PrepareCopy();
   m_spImpl->m_dt -= span.m_spImpl->m_span;
   return *this;
}

DateTime DateTime::operator-(const TimeSpan& rhs) const throw()
{
   DateTime dt(*this);
   dt -= rhs;
   return dt;
}

bool DateTime::operator==(const DateTime& rhs) const throw()
{
   ATLASSERT(m_spImpl != NULL);
   return m_spImpl->m_dt == rhs.m_spImpl->m_dt;
}

bool DateTime::operator!=(const DateTime& rhs) const throw()
{
   ATLASSERT(m_spImpl != NULL);
   return m_spImpl->m_dt != rhs.m_spImpl->m_dt;
}

bool DateTime::operator>(const DateTime& rhs) const throw()
{
   ATLASSERT(m_spImpl != NULL);
   return m_spImpl->m_dt > rhs.m_spImpl->m_dt;
}

bool DateTime::operator<(const DateTime& rhs) const throw()
{
   ATLASSERT(m_spImpl != NULL);
   return m_spImpl->m_dt < rhs.m_spImpl->m_dt;
}

CString DateTime::FormatISO8601(T_enISO8601Format enFormat, bool bBasic, const TimeZone& tz) const throw()
{
   CString cszDate;
   switch(enFormat)
   {
   case formatY:  return Format(_T("%Y"), tz);
   case formatYM: return Format(bBasic ? _T("%Y%m") : _T("%Y-%m"), tz);
   case formatYMD: return Format(bBasic ? _T("%Y%m%d") : _T("%Y-%m-%d"), tz);
   case formatYMD_HM_Z:
      cszDate = Format(bBasic ? _T("%Y%m%dT%H%M") : _T("%Y-%m-%dT%H:%M"), tz);
      break;

   case formatYMD_HMS_Z:
      cszDate = Format(bBasic ? _T("%Y%m%dT%H%M%S") : _T("%Y-%m-%dT%H:%M:%S"), tz);
      break;

   case formatYMD_HMSF_Z:
      {
         cszDate = Format(bBasic ? _T("%Y%m%dT%H%M%S") : _T("%Y-%m-%dT%H:%M:%S"), tz);

         CString cszFraction;
         cszFraction.Format(_T(".%03u"), Millisecond());
         cszDate += cszFraction;
      }
      break;
   }

   // add timezone
   if (tz.StandardName() == _T("UTC"))
      cszDate += _T("Z");
   else
   {
      TimeSpan spTimezone = tz.GetUtcOffset(*this);
      bool bNegative = spTimezone < TimeSpan(0, 0, 0, 0);

      TimeSpan spTimezoneAbs = bNegative ? -spTimezone : spTimezone;

      CString cszTimezone;
      cszTimezone.Format(bBasic ? _T("%c%02u%02u") : _T("%c%02u:%02u"),
         !bNegative ? _T('+') : _T('-'),
         spTimezoneAbs.Hours(),
         spTimezoneAbs.Minutes());

      cszDate += cszTimezone;
   }

   return cszDate;
}

CString DateTime::Format(const CString& cszFormat, const TimeZone& tz) const throw()
{
   ATLASSERT(m_spImpl != NULL);

   if (m_spImpl->m_dt.is_not_a_date_time())
      return _T("");

   // calculate offset to UTC
   TimeSpan tsTimezone = tz.GetUtcOffset(*this);

   struct tm tmTemp = boost::posix_time::to_tm(m_spImpl->m_dt + tsTimezone.m_spImpl->m_span);

   CString cszDateTime;
   LPTSTR pszBuffer = cszDateTime.GetBufferSetLength(256);
   _tcsftime(pszBuffer, cszDateTime.GetLength(), cszFormat, &tmTemp);
   cszDateTime.ReleaseBuffer();

   return cszDateTime;
}

void DateTime::ParseISO8601(const CString& cszISO8601Timestamp) const throw()
{
   if (cszISO8601Timestamp.IsEmpty())
   {
      m_spImpl->m_dt = boost::date_time::not_a_date_time;
      return;
   }

   ISO8601Parser p(cszISO8601Timestamp);
   if (p.IsValid())
      m_spImpl->m_dt = p.Get();
}

void DateTime::PrepareCopy() throw()
{
   ATLASSERT(m_spImpl != NULL);

   if (!m_spImpl.unique())
      m_spImpl.reset(new DateTimeImpl(m_spImpl->m_dt));
}
