//
// PartyVideoDJ - YouTube crossfading app
// Copyright (C) 2008-2017 Michael Fink
//
/// \file FullscreenHelper.cpp Helper for showing app in full screen
//
#include "StdAfx.h"
#include "FullscreenHelper.hpp"

bool FullscreenHelper::IsFullscreen(HWND hwnd)
{
   DWORD dwStyle = ::GetWindowLong(hwnd, GWL_STYLE);
   return (dwStyle & WS_OVERLAPPEDWINDOW) == 0;
}

/// from Raymond Chen's blog: "How do I switch a window between normal and fullscreen?"
/// \see http://blogs.msdn.com/b/oldnewthing/archive/2010/04/12/9994016.aspx
void FullscreenHelper::ToggleFullscreen(HWND hwnd)
{
   DWORD dwStyle = ::GetWindowLong(hwnd, GWL_STYLE);
   if (dwStyle & WS_OVERLAPPEDWINDOW)
   {
      // to fullscreen
      MONITORINFO mi = { sizeof(mi) };
      if (::GetWindowPlacement(hwnd, &g_wpPrev) &&
         GetMonitorInfo(MonitorFromWindow(hwnd,
            MONITOR_DEFAULTTOPRIMARY), &mi)) {
         ::SetWindowLong(hwnd, GWL_STYLE,
            dwStyle & ~WS_OVERLAPPEDWINDOW);
         ::SetWindowPos(hwnd, HWND_TOP,
            mi.rcMonitor.left, mi.rcMonitor.top,
            mi.rcMonitor.right - mi.rcMonitor.left,
            mi.rcMonitor.bottom - mi.rcMonitor.top,
            SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
      }
   }
   else
   {
      // to windowed
      ::SetWindowLong(hwnd, GWL_STYLE,
         dwStyle | WS_OVERLAPPEDWINDOW);
      ::SetWindowPlacement(hwnd, &g_wpPrev);
      ::SetWindowPos(hwnd, NULL, 0, 0, 0, 0,
         SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
         SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
   }
}
