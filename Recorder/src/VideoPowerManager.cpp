#include "stdafx.h"
#include <ulib/wince/VideoPowerManager.hpp>

// from MSDN, http://msdn.microsoft.com/library/default.asp?url=/library/en-us/dnppcgen/html/ppc_layoff.asp

// GDI Escapes for ExtEscape()
#define QUERYESCSUPPORT    8
 
// The following are unique to CE
#define GETVFRAMEPHYSICAL   6144
#define GETVFRAMELEN    6145
#define DBGDRIVERSTAT    6146
#define SETPOWERMANAGEMENT   6147
#define GETPOWERMANAGEMENT   6148

/// enum for the different video power states; used in VIDEO_POWER_STATE::PowerState
typedef enum _VIDEO_POWER_STATE
{
   VideoPowerOn = 1,
   VideoPowerStandBy,
   VideoPowerSuspend,
   VideoPowerOff
} VIDEO_POWER_STATE, *PVIDEO_POWER_STATE;

typedef struct _VIDEO_POWER_MANAGEMENT
{
   ULONG Length;        ///< length of this struct
   ULONG DPMSVersion;   ///< DPMS version (alway set to 1)
   ULONG PowerState;    ///< power state value from enum VIDEO_POWER_STATE
} VIDEO_POWER_MANAGEMENT, *PVIDEO_POWER_MANAGEMENT;

CVideoPowerManager::CVideoPowerManager()
:m_bDisplayOff(false)
{
}

CVideoPowerManager::~CVideoPowerManager()
{
   // if the display was not off, switch it back on
   if (m_bDisplayOff)
      SwitchOffDisplay(false);
}

bool CVideoPowerManager::IsAvailDisplayOff()
{
   int iESC = SETPOWERMANAGEMENT;
   HDC dc = ::GetDC(NULL);

   // query for SETPOWERMANAGEMENT support
   bool bAvail = ExtEscape(dc, QUERYESCSUPPORT,
      sizeof(int), reinterpret_cast<LPCSTR>(&iESC),
      0, NULL) != 0;

   ReleaseDC(NULL, dc);

   return bAvail;
}

bool CVideoPowerManager::SwitchOffDisplay(bool bDisplayOff)
{
   // set up struct
   VIDEO_POWER_MANAGEMENT vpm;
   ZeroMemory(&vpm, sizeof(vpm));
   vpm.Length = sizeof(vpm);

   vpm.DPMSVersion = 0x0001;
   vpm.PowerState = bDisplayOff ? VideoPowerOff : VideoPowerOn;

   HDC dc = ::GetDC(NULL);

   // power on/off the display
   int iRet = ExtEscape(dc, SETPOWERMANAGEMENT,
      vpm.Length, reinterpret_cast<LPCSTR>(&vpm),
      0, NULL);

   /// remember current state
   if (iRet > 0)
      m_bDisplayOff = bDisplayOff;

   ::ReleaseDC(NULL, dc);

   return iRet > 0;
}
