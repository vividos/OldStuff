//
// ulib - a collection of useful classes
// Copyright (C) 2006-2014 Michael Fink
//
/// \file SystemTime.hpp SYSTEMTIME wrapper
//
#pragma once

/// thin SYSTEMTIME wrapper
class SystemTime : public SYSTEMTIME
{
public:
   /// default ctor
   SystemTime() throw()
   {
      ZeroMemory(this, sizeof(*this));
   }

   /// ctor that takes filetime
   SystemTime(const FILETIME& fileTime) throw()
   {
      ::FileTimeToSystemTime(&fileTime, this);
   }

   /// less-than operator to store SystemTime in an ordered container
   bool operator<(const SystemTime& st) const throw()
   {
      return wYear < st.wYear &&
         wMonth < st.wMonth &&
         wDay < st.wDay &&
         wHour < st.wHour &&
         wMinute < st.wMinute &&
         wSecond < st.wSecond &&
         wMilliseconds < st.wMilliseconds;
   }

   /// returns true when system time is empty
   bool IsEmpty() const throw()
   {
      return wYear == 0 &&
         wMonth == 0 &&
         wDay == 0 &&
         wHour == 0 &&
         wMinute == 0 &&
         wSecond == 0 &&
         wMilliseconds == 0;
   }
};
