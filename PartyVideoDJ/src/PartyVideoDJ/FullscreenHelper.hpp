//
// PartyVideoDJ - YouTube crossfading app
// Copyright (C) 2008-2017 Michael Fink
//
/// \file FullscreenHelper.hpp Helper for showing app in full screen
//
#pragma once

class FullscreenHelper
{
public:
   FullscreenHelper()
   {
      g_wpPrev.length = sizeof(g_wpPrev);
   }

   bool IsFullscreen(HWND hwnd);

   void ToggleFullscreen(HWND hwnd);

private:
   WINDOWPLACEMENT g_wpPrev;
};
