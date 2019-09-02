#include "stdafx.h"
#include <ulib/wince/BluetoothActivator.hpp>

/* TODO support Broadcom stack
btcoreif.dll -> widcomm stack
BTM_SetConnectability
BTM_SetDiscoverability
*/

//#include <bthutil.h>
#pragma comment(lib, "bthutil.lib")

CBluetoothActivator::CBluetoothActivator(DWORD dwMode)
{
   ATLVERIFY(ERROR_SUCCESS == ::BthGetMode(&m_dwLastMode));
   ATLVERIFY(ERROR_SUCCESS == ::BthSetMode(dwMode));
}

CBluetoothActivator::~CBluetoothActivator()
{
   ATLVERIFY(ERROR_SUCCESS == ::BthSetMode(m_dwLastMode));
}

DWORD CBluetoothActivator::GetCurrentMode()
{
   DWORD dwMode;
   ATLVERIFY(ERROR_SUCCESS == ::BthGetMode(&dwMode));
   return dwMode;
}

bool CBluetoothActivator::IsBluetoothAvail()
{
   HMODULE hMod = ::LoadLibrary(_T("bthutil.dll"));
   ::FreeLibrary(hMod);
   return hMod != NULL;
}
