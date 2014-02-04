//
// ulib - a collection of useful classes
// Copyright (C) 2006-2014 Michael Fink
//
/// \file DateTime.hpp date/time class
//
#pragma once

// includes
#include <ulib/config/AutoLink.hpp>
#include <memory>
#include <vector>

// forward references
class DateTime;
class TimeSpan;

/// represents a single timezone
class ULIB_DECLSPEC TimeZone
{
public:
   /// returns UTC timezone
   static TimeZone UTC() throw();

   /// returns current system's timezone
   static TimeZone System();

   /// enumerates all timezones in the system
   static std::vector<TimeZone> EnumerateTimezones();

   /// returns timezone name during daylight savings time
   CString DaylightName() const throw() { return m_tzi.DaylightName; }

   /// returns timezone name during standard time
   CString StandardName() const throw() { return m_tzi.StandardName; }

   /// returns display name for timezone when enumerating all system timezones
   CString DisplayName() const throw() { return m_cszDisplayName; }

   /// gets UTC offset for given date/time
   TimeSpan GetUtcOffset(const DateTime& dt) const;

   /// returns if date/time is in daylight savings time
   bool IsDaylightSavingTime(const DateTime& dt) const;

   /// compare function for std::sort; sorts by bias
   static bool CompareLessBias(const TimeZone& lhs, const TimeZone& rhs);

   /// compare function for std::sort; sorts by bias
   static bool CompareGreaterBias(const TimeZone& lhs, const TimeZone& rhs);

private:
   TimeZone();

   /// calculates transition time for daylight savings time (index=0) or standard time (index=1)
   DateTime CalculateTransitionDate(unsigned int uiYear, unsigned int uiIndex) const;

private:
   /// display name
   CString m_cszDisplayName;

   /// timezone info
   TIME_ZONE_INFORMATION m_tzi;
};


/// represents a relative time span
class ULIB_DECLSPEC TimeSpan
{
public:
   /// date/time span status values
   enum T_enStatus { valid = 0, invalid, min, max };

   /// default ctor
   TimeSpan() throw();

   /// ctor; takes date/time span components
   TimeSpan(int iHours, int iMins, int iSecs, int iMillisecs = 0) throw();

   /// ctor; initialize with min or max status
   TimeSpan(T_enStatus status);

   /// copy ctor
   TimeSpan(const TimeSpan& dt);

   /// assignment operator
   TimeSpan& operator=(const TimeSpan& rhs);

   // properties

   /// returns date/time status
   T_enStatus Status() const throw();

   /// component hours in span (-23 to 23)
   int Hours() const throw();
   /// component minutes in span (-59 to 59)
   int Minutes() const throw();
   /// component seconds in span (-59 to 59)
   int Seconds() const throw();
   /// component milliseconds in span (-999 to 999)
   int Milliseconds() const throw();

   /// span in hours (about -8.77e7 to 8.77e6)
   double TotalHours() const throw();
   /// span in minutes (about -5.26e9 to 5.26e9)
   double TotalMinutes() const throw();
   /// span in seconds (about -3.16e11 to 3.16e11)
   double TotalSeconds() const throw();
   /// span in milliseconds
   double TotalMilliseconds() const throw();

   // operators
   TimeSpan operator+(const TimeSpan& rhs) const throw(); ///< add operator
   TimeSpan operator-(const TimeSpan& rhs) const throw(); ///< subtract operator
   TimeSpan operator*(int iFactor) const throw();          ///< multiply by scalar operator
   TimeSpan operator/(int iFactor) const throw();          ///< divide by scalar operator
   TimeSpan& operator+=(const TimeSpan span) throw();     ///< add inplace operator
   TimeSpan& operator-=(const TimeSpan span) throw();     ///< subtract inplace operator
   TimeSpan& operator*=(int iFactor) throw();              ///< multiply by scalar inplace operator
   TimeSpan& operator/=(int iFactor) throw();              ///< divide by scalar inplace operator
   TimeSpan operator-() const throw();                     ///< subtract two time spans operator

   bool operator==(const TimeSpan& rhs) const throw();     ///< equal operator
   bool operator!=(const TimeSpan& rhs) const throw();     ///< unequal operator
   bool operator>(const TimeSpan& rhs) const throw();      ///< greater operator
   bool operator<(const TimeSpan& rhs) const throw();      ///< less operator
   bool operator<=(const TimeSpan& rhs) const throw() { return !operator>(rhs); }  ///< greater or equal operator
   bool operator>=(const TimeSpan& rhs) const throw() { return !operator<(rhs); }  ///< less or equal operator

   /// sets time span components
   void SetDateTimeSpan(int iHours, int iMins, int iSecs, int iMillisecs) throw();

   /// format time span as ISO 8601 (in "hh:mm:ss" format)
   CString FormatISO8601() const throw();

private:
   /// prepares another copy of the object when date/time value is about to be modified
   void PrepareCopy() throw();

private:
   friend class DateTime;

   /// implementation
   std::shared_ptr<class TimeSpanImpl> m_spImpl;
};


/// represents a date/time point
class ULIB_DECLSPEC DateTime
{
public:
   /// date/time status values
   enum T_enStatus { valid = 0, invalid, min, max };

   /// default ctor
   DateTime();

   /// ctor; takes date/time components
   DateTime(unsigned int uiYear, unsigned int uiMonth, unsigned int uiDay,
      unsigned int uiHour, unsigned int uiMinute, unsigned int uiSecond,
      unsigned int uiMillisecond = 0);

   /// ctor; initialize with min or max status
   DateTime(T_enStatus status);

   /// copy ctor
   DateTime(const DateTime& dt);

   /// assignment operator
   DateTime& operator=(const DateTime& rhs);

   /// returns current date/time
   static DateTime Now() throw();

   /// returns current date with time part set to 00:00:00
   static DateTime Today() throw();

   /// returns max time value
   static DateTime MaxValue() throw() { return DateTime(DateTime::max); }

   /// returns min time value
   static DateTime MinValue() throw() { return DateTime(DateTime::min); }

   /// sets date/time components
   void SetDateTime(unsigned int uiYear, unsigned int uiMonth, unsigned int uiDay,
      unsigned int uiHour, unsigned int uiMinute, unsigned int uiSecond,
      unsigned int uiMillisecond = 0) throw();

   // properties

   /// returns date/time status
   T_enStatus Status() const throw();

   /// returns year
   unsigned int Year() const throw();
   /// month of year; 1 = january
   unsigned int Month() const throw();
   /// day of month (1-31)
   unsigned int Day() const throw();

   /// hour in day (0-23)
   unsigned int Hour() const throw();
   /// minute in hour (0-59)
   unsigned int Minute() const throw();
   /// second in minute (0-59)
   unsigned int Second() const throw();
   /// millisecond in second (0-999)
   unsigned int Millisecond() const throw();

   /// day of week; 0: sunday; ... 6: saturday
   unsigned int DayOfWeek() const throw();
   /// day of year; 1-based
   unsigned int DayOfYear() const throw();
   /// returns time part of date
   TimeSpan TimeOfDay() throw();

   // operators

   DateTime operator+(const TimeSpan& rhs) const throw(); ///< add operator
   DateTime operator-(const TimeSpan& rhs) const throw(); ///< subtract operator
   DateTime& operator+=(const TimeSpan& span) throw();    ///< add inplace operator
   DateTime& operator-=(const TimeSpan& span) throw();    ///< subtract inplace operator
   TimeSpan operator-(const DateTime& rhs) const throw(); ///< subtracts two date/times

   bool operator==(const DateTime& rhs) const throw();  ///< equal operator
   bool operator!=(const DateTime& rhs) const throw();  ///< unequal operator
   bool operator>(const DateTime& rhs) const throw();   ///< greater operator
   bool operator<(const DateTime& rhs) const throw();   ///< less operator
   bool operator<=(const DateTime& rhs) const throw() { return !operator>(rhs); } ///< greater or equal operator
   bool operator>=(const DateTime& rhs) const throw() { return !operator<(rhs); } ///< less or equal operator

   /// ISO 8601 format
   enum T_enISO8601Format
   {
      formatY,       ///< year only
      formatYM,      ///< year and month
      formatYMD,     ///< year and month
      formatYMD_HM_Z,   ///< year, month, day, hour, minute and timezone offset
      formatYMD_HMS_Z,  ///<year, month, day, hour, minute, second and timezone offset
      formatYMD_HMSF_Z, ///< full date/time with fraction and timezone offset
   };

   /// formats date/time using ISO 8601 format
   CString FormatISO8601(T_enISO8601Format enFormat = formatYMD_HMS_Z, bool bBasic = false,
      const TimeZone& tz = TimeZone::System()) const throw();

   /// formats date/time with given format, see _tcsftime
   CString Format(const CString& cszFormat, const TimeZone& tz = TimeZone::System()) const throw();

   /// parses ISO 8601 formatted date/time
   void ParseISO8601(const CString& cszISO8601Timestamp) const throw();

private:
   /// prepares another copy of the object when date/time value is about to be modified
   void PrepareCopy() throw();

private:
   /// implementation
   std::shared_ptr<class DateTimeImpl> m_spImpl;
};
