//
// ulib - a collection of useful classes
// Copyright (C) 2006,2007 Michael Fink
//
//! \file DecoratedName.cpp decorated name resolution
//

// needed includes
#include "stdafx.h"
#ifndef _WIN32_WCE
#include <ulib/debug/DecoratedName.hpp>

#ifdef UNICODE
#define DBGHELP_TRANSLATE_TCHAR
#endif
#pragma warning(push)
#pragma warning(disable: 4091) // 'typedef ': ignored on left of '' when no variable is declared
#include <dbghelp.h>
#pragma warning(pop)
//#include <ulib/CriticalSection.hpp>
#include <vector>

#pragma comment(lib, "dbghelp.lib")

/*! \todo move critical section elsewhere */
CString Debug::DecoratedName::GetUndecorated() const throw()
{
   // the description of UnDecorateSymbolName explains that the function should be called
   // from a single thread at a time only, so protect access with a critical section.
//   static CCriticalSection cs;
//   cs.Enter();

   std::vector<TCHAR> vecBuffer(256, (char)0);

   // note: since the T2CA (and all other) macros are not loop-safe, this could lead to problems
   USES_CONVERSION;

   // since we can't find out the length of the buffer needed, increase up to 1024 until
   // we successfully get the string.
   DWORD dwResult = 0;
   do
   {
      dwResult = UnDecorateSymbolName(
         m_cszDecoratedName,
         &vecBuffer[0], static_cast<DWORD>(vecBuffer.size()-1), UNDNAME_COMPLETE);

      if (dwResult == 0 && vecBuffer.size() < 1024)
      {
         // enlarge the buffer
         vecBuffer.resize(vecBuffer.size() * 2, (char)0);
      }

   } while (dwResult == 0);

//   cs.Leave();

   return CString(dwResult == 0 ? _T("") : &vecBuffer[0]);
}

#endif // _WIN32_WCE
