//
// PartyVideoDJ - YouTube crossfading app
// Copyright (C) 2008-2017 Michael Fink
//
/// \file Screensaver.hpp Screensaver control
//
#pragma once

/// controls screen saver
/// http://support.microsoft.com/kb/126627
class Screensaver
{
public:
   Screensaver()
      :m_bInitiallyEnabled(true),
      m_bEnabled(true)
   {
   }

   ~Screensaver()
   {
      // restore initial state
      if (m_bInitiallyEnabled != m_bEnabled)
      {
         if (m_bInitiallyEnabled)
            Enable();
         else
            Disable();
      }
   }

   /// returns if the screen saver is enabled
   static bool IsEnabled()
   {
      // note: we can only check if it's possible that a screen saver could show
      // http://support.microsoft.com/kb/318781
      BOOL bEnabled = FALSE;
      ::SystemParametersInfo(SPI_GETSCREENSAVEACTIVE, 0, (LPVOID)&bEnabled, 0);
      return bEnabled != FALSE;
   }

   void Disable()
   {
      ::SystemParametersInfo(SPI_SETSCREENSAVEACTIVE, FALSE, 0, SPIF_SENDWININICHANGE);
      m_bEnabled = false;
   }

   void Enable()
   {
      ::SystemParametersInfo(SPI_SETSCREENSAVEACTIVE, TRUE, 0, SPIF_SENDWININICHANGE);
      m_bEnabled = true;
   }

   void Start(HWND hWnd)
   {
      ::PostMessage(hWnd, WM_SYSCOMMAND, SC_SCREENSAVE, 0);
   }

private:
   bool m_bInitiallyEnabled;
   bool m_bEnabled;
};
