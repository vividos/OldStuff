// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#pragma once

// Change this value to use different versions
#define WINVER 0x0420
/*
#define _SECURE_ATL 1
#include <atlbase.h>

#if _ATL_VER < 0x0700
#  include <atlmisc.h>
#else
  //#define _WTL_NO_CSTRING
  //#define _WTL_USE_CSTRING
#  include <atlstr.h>
#  define _WTL_NO_CSTRING
#endif

#include <atlapp.h>

extern CAppModule _Module;

#include <atlwin.h>
*/

//#include <ulib/config/atl.hpp>
#include <ulib/config/wtl.hpp>


#if _WIN32_WCE >= 0x420
#include <tpcshell.h>
#endif
#include <aygshell.h>
#pragma comment(lib, "aygshell.lib")

/*
#include <atlframe.h>
#include <atlctrls.h>

#ifndef min
#define min(a,b) ((a)<(b)?(a):(b))
#endif
#ifndef max
#define max(a,b) ((a)>(b)?(a):(b))
#endif
#include <atlmisc.h>
#undef min
#undef max
*/

#define _WTL_CE_NO_ZOOMSCROLL
#define _WTL_CE_NO_FULLSCREEN
#include <atlwince.h>

//#include <ulib/config/config.hpp>
#include <ulib/Exception.hpp>

#include <exception>
#include <boost/shared_ptr.hpp>


namespace ATL
{
   class CMutex
   {
   public:
      CMutex()
         :m_hMutex(::CreateMutex(NULL, FALSE, NULL))
      {
      }
      ~CMutex()
      {
         ATLVERIFY(TRUE == ::CloseHandle(m_hMutex));
      }

      operator HANDLE(){ return m_hMutex; }

   private:
      HANDLE m_hMutex;
   };

   class CMutexLock
   {
   public:
      CMutexLock(CMutex& mtx)
         :m_mtx(mtx)
      {
         ::WaitForSingleObject(mtx, INFINITE);
      }
      ~CMutexLock()
      {
         ::ReleaseMutex(m_mtx);
      }
   private:
      CMutex& m_mtx;
   };
}
