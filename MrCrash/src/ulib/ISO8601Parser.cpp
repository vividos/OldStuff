//
// ulib - a collection of useful classes
// Copyright (C) 2006-2014 Michael Fink
//
/// \file ISO8601Parser.cpp ISO 8601 date parser
//

// includes
#include "stdafx.h"
#include "ISO8601Parser.hpp"

ISO8601Parser::ISO8601Parser(LPCTSTR pszTimestamp)
:m_dt(boost::date_time::not_a_date_time)
{
   std::wstring strRemaining;
   if (!ParseDateTime(pszTimestamp, strRemaining))
      return;

   if (!ParseTimezoneOffset(strRemaining))
      return;

   if (m_tzOffset.is_not_a_date_time())
   {
      m_dt = boost::date_time::not_a_date_time;
      return;
   }
}

bool ISO8601Parser::ParseDateTime(LPCTSTR pszTimestamp, std::wstring& strRemaining)
{
   // convert to ptime using time input facet
   // http://stackoverflow.com/questions/2838524/use-boost-date-time-to-parse-and-create-http-dates
   //typedef boost::posix_time::wtime_input_facet time_input_facet;
   typedef boost::local_time::wlocal_time_input_facet time_input_facet;

   // note: inputFacet is not deleted here; it is done somewhere inside the stream classes
   time_input_facet* inputFacet =
      //new time_input_facet(L"%Y-%m-%dT%H:%M:%S%F%Q");
      new time_input_facet(L"%Y-%m-%dT%H:%M:%S%F");
   // note: we don't use
   //    inputFacet->set_iso_extended_format();
   // here, since it doesn't include the T divider

   std::wstringstream ss;
   ss.imbue(std::locale(ss.getloc(), inputFacet));

   ss.str(pszTimestamp);

   boost::local_time::local_date_time localDateTime(
      boost::local_time::local_sec_clock::local_time(boost::local_time::time_zone_ptr()));

   ss >> localDateTime;

   if (ss.fail())
      return false;

   // convert to ptime
   m_dt = localDateTime.utc_time();

   std::getline(ss, strRemaining);

   return true;
}

bool ISO8601Parser::ParseTimezoneOffset(std::wstring& strRemaining)
{
   if (strRemaining.empty())
      return false;

   // check next char
   TCHAR chNext = strRemaining[0];
   if (chNext == _T('Z'))
   {
      // UTC
      m_tzOffset = boost::posix_time::time_duration(0, 0, 0);
   }
   else if (chNext == _T('+') || chNext == _T('-'))
   {
      // numeric offset
      USES_CONVERSION;
      std::string strRemaining2(T2CA(strRemaining.c_str()));
      m_tzOffset = boost::posix_time::duration_from_string(strRemaining2);
   }

   return !m_tzOffset.is_not_a_date_time();
}
