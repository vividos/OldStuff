//
// Recorder - a GPS logger app for Windows Mobile
// Copyright (C) 2006-2019 Michael Fink
//
/// \file BacklightManager.cpp Backlight manager
//
#include "StdAfx.h"
#include "BacklightManager.hpp"
#ifdef _WIN32_WCE
#include <pm.h>
#endif

CBacklightManager::CBacklightManager(EBacklightLevel enBacklightLevel) throw()
   :m_hPower(NULL)
{
#ifdef _WIN32_WCE
   m_hPower = SetPowerRequirement(_T("BKL1:"),
      static_cast<CEDEVICE_POWER_STATE>(enBacklightLevel),
      POWER_NAME, NULL, 0);
#endif
}

CBacklightManager::~CBacklightManager() throw()
{
#ifdef _WIN32_WCE
   ReleasePowerRequirement(m_hPower);
#endif
}
