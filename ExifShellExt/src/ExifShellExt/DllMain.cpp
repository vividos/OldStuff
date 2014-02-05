//
// ExifShellExt - Exif data shell extension
// Copyright (C) 2008 Michael Fink
//
/// \file DllMain.cpp COM DLL functions
//

// includes
#include "stdafx.h"
#include "resource.h"
#include <initguid.h>
#include "ExifShellExt_h.h"
#include "ExifShellExt_i.c"
#include "ExifShellExtension.hpp"

CComModule _Module;

BEGIN_OBJECT_MAP(ObjectMap)
OBJECT_ENTRY(CLSID_ExifShellExtension, ExifShellExtension)
END_OBJECT_MAP()

extern "C"
BOOL APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID)
{
   if (dwReason == DLL_PROCESS_ATTACH)
   {
      _Module.Init(ObjectMap, hInstance, &LIBID_ExifShellExtLib);
      DisableThreadLibraryCalls(hInstance);
   }
   else if (dwReason == DLL_PROCESS_DETACH)
      _Module.Term();

   return TRUE;    // ok
}

/////////////////////////////////////////////////////////////////////////////
// Used to determine whether the DLL can be unloaded by OLE

STDAPI DllCanUnloadNow()
{
   return (_Module.GetLockCount()==0) ? S_OK : S_FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// Returns a class factory to create an object of the requested type

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
   return _Module.GetClassObject(rclsid, riid, ppv);
}

/////////////////////////////////////////////////////////////////////////////
// DllRegisterServer - Adds entries to the system registry

STDAPI DllRegisterServer()
{
   // If we're on NT, add ourselves to the list of approved shell extensions.

   // Note that you should *NEVER* use the overload of CRegKey::SetValue with
   // 4 parameters.  It lets you set a value in one call, without having to 
   // call CRegKey::Open() first.  However, that version of SetValue() has a
   // bug in that it requests KEY_ALL_ACCESS to the key.  That will fail if the
   // user is not an administrator.  (The code should request KEY_WRITE, which
   // is all that's necessary.)

   if (0 == (GetVersion() & 0x80000000UL))
   {
      CRegKey reg;
      LONG    lRet;

      lRet = reg.Open(HKEY_LOCAL_MACHINE,
         _T("Software\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved"),
         KEY_SET_VALUE);

      if (ERROR_SUCCESS != lRet)
         return E_ACCESSDENIED;

      lRet = reg.SetStringValue(_T("Exif shell extension"), _T("{A4220261-FF2C-4a1e-A7BB-1ED6E90BFA52}"));

      if (ERROR_SUCCESS != lRet)
         return E_ACCESSDENIED;
   }

   // registers object, typelib and all interfaces in typelib
   return _Module.RegisterServer(FALSE);
}

/////////////////////////////////////////////////////////////////////////////
// DllUnregisterServer - Removes entries from the system registry

STDAPI DllUnregisterServer()
{
   // If we're on NT, remove ourselves from the list of approved shell extensions.
   // Note that if we get an error along the way, I don't bail out since I want
   // to do the normal ATL unregistration stuff too.

   if (0 == (GetVersion() & 0x80000000UL))
   {
      CRegKey reg;
      LONG    lRet;

      lRet = reg.Open(HKEY_LOCAL_MACHINE,
         _T("Software\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved"),
         KEY_SET_VALUE);

      if (ERROR_SUCCESS == lRet)
      {
         lRet = reg.DeleteValue(_T("{A4220261-FF2C-4a1e-A7BB-1ED6E90BFA52}"));
      }
   }

   return _Module.UnregisterServer(FALSE);
}
