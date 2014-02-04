//
// ulib - a collection of useful classes
// Copyright (C) 2006-2014 Michael Fink
//
/// \file ISO8601Parser.hpp ISO 8601 date parser
//
#pragma once

// include Boost.DateTime
#define BOOST_DATE_TIME_NO_LIB

#pragma warning(push)
#pragma warning(disable: 4244) // 'argument' : conversion from 'T1' to 'T2', possible loss of data
#pragma warning(disable: 4245) // 'initializing' : conversion from 'T1' to 'T2', signed/unsigned mismatch
#include <boost/date_time.hpp>
#pragma warning(pop)

/// \brief parser for ISO 8601 date formats
class ISO8601Parser
{
public:
   /// ctor; starts parsing
   ISO8601Parser(LPCTSTR pszTimestamp);

   /// returns if parsed date/time is valid
   bool IsValid() const
   {
      return !m_dt.is_not_a_date_time() && !m_tzOffset.is_not_a_date_time();
   }

   /// returns parsed date/time
   boost::posix_time::ptime Get() const
   {
      return m_dt - m_tzOffset;
   }

private:
   /// parses date/time part
   bool ParseDateTime(LPCTSTR pszTimestamp, std::wstring& strRemaining);

   /// parses timezone offset
   bool ParseTimezoneOffset(std::wstring& strRemaining);

private:
   /// date/time part
   boost::posix_time::ptime m_dt;

   /// timezone offset
   boost::posix_time::time_duration m_tzOffset;
};
