//
// MultiplayerOnlineGame - multiplayer game project
// Copyright (C) 2008-2014 Michael Fink
//
/// \file FrameCounter.hpp Frame per second counter
//
#pragma once

// includes
#include <SDL.h>

/// frames per second counter class
class FrameCounter
{
public:
   /// ctor
   FrameCounter()
      :m_uiFrames(0)
   {
      m_uiStart = SDL_GetTicks();
   }

   /// call when next frame is rendered
   void OnNextFrame()
   {
      m_uiFrames++;
   }

   /// returns updated count; value only valid when returning true
   bool GetUpdatedCount(double& dFramesPerSecond)
   {
      dFramesPerSecond = 0.0;

      Uint32 uiNow = SDL_GetTicks();
      if (uiNow - m_uiStart < 500)
         return false;

      dFramesPerSecond = m_uiFrames*1000.0/(uiNow-m_uiStart);

      // restart counting
      m_uiFrames = 0;
      m_uiStart = uiNow;

      return true;
   }

private:
   Uint32 m_uiStart;          ///< tick count at start of measurement
   unsigned int m_uiFrames;   ///< number of frames counted so far
};
