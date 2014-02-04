//
// MrCrash - crash info reporting tool
// Copyright (C) 2005-2014 Michael Fink
//
/// \file DebugEvent.hpp Debug event
//
#pragma once

/// debug event
class DebugEvent
{
public:
   DebugEvent(const CString& cszText) throw()
      :m_cszText(cszText)
   {
      GetLocalTime(&m_stTime);
   }

   // get methods

   const CString& Text() const throw() { return m_cszText; }
   const SystemTime& Time() const throw() { return m_stTime; }

private:
   CString m_cszText;
   SystemTime m_stTime;
};
