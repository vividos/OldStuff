#include "StdAfx.h"
#include "BacklightManager.hpp"
#include <pm.h>

CBacklightManager::CBacklightManager(EBacklightLevel enBacklightLevel) throw()
:m_hPower(NULL)
{
   m_hPower = SetPowerRequirement(_T("BKL1:"),
      static_cast<CEDEVICE_POWER_STATE>(enBacklightLevel),
      POWER_NAME, NULL, 0);
}

CBacklightManager::~CBacklightManager() throw()
{
   ReleasePowerRequirement(m_hPower);
}
