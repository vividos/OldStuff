//
// Recorder - a GPS logger app for Windows Mobile
// Copyright (C) 2006-2019 Michael Fink
//
/// \file BluetoothActivator.hpp Bluetooth activator
//
#pragma once

#include <bthutil.h>

/// \brief activates bluetooth connection
/// The class activates a certain bluetooth mode and on destruction restores the
/// previous mode. The bluetooth connection can be in one of the following modes:
/// - BTH_CONNECTABLE: turns on bluetooth radio
/// - BTH_DISCOVERABLE: turns on bluetooth and makes device discoverable
/// - BTH_POWER_OFF: turns off bluetooth
/// All enums are available via <bthutil.h>
class CBluetoothActivator
{
public:
   /// ctor; activates bluetooth mode
	CBluetoothActivator(DWORD dwMode = BTH_CONNECTABLE);
   /// dtor; restores the last mode
	~CBluetoothActivator();

   /// returns current mode
   static DWORD GetCurrentMode();

   /// returns if bluetooth is available
   static bool IsBluetoothAvail();

private:
   /// saved mode
   DWORD m_dwLastMode;
};
