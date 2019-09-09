//
// Recorder - a GPS logger app for Windows Mobile
// Copyright (C) 2006-2019 Michael Fink
//
/// \file VideoPowerManager.hpp Video power manager
//
#pragma once

/// video power manager
class CVideoPowerManager
{
public:
   CVideoPowerManager();
   ~CVideoPowerManager();

   /// returns if display switching off is available
   static bool IsAvailDisplayOff();

   /// switches display on/off
   bool SwitchOffDisplay(bool bDisplayOff);

private:
   /// remembers display off state
   bool m_bDisplayOff;
};
