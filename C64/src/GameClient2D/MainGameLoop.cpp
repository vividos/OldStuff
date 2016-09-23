//
// MultiplayerOnlineGame - multiplayer game project
// Copyright (C) 2008-2016 Michael Fink
//
/// \file MainGameLoop.cpp Main game loop
//

// includes
#include "StdAfx.h"
#include "MainGameLoop.hpp"
#include "FrameCounter.hpp"

/// user code for event dispatch; see MainGameLoop::DispatchInEventLoop()
const int c_iUserEventDispatch = 0;

MainGameLoop::MainGameLoop(bool bUpdateFrameCount, const CString& cszClientName)
:m_framesPerSecond(25),
 m_bUpdateFrameCount(bUpdateFrameCount),
 m_cszClientName(cszClientName),
 m_bExitLoop(false),
 m_bAppActive(true),
 m_bMouseFocus(true),
 m_bInputFocus(true)
{
}

void MainGameLoop::Run()
{
   FrameCounter fc;

   // main game loop
   while (!m_bExitLoop)
   {
      OnTick();

      ProcessEvents();
      if (m_bExitLoop)
         break;

      if (!m_bAppActive)
      {
         // as we're not visible, just wait for next event
         SDL_WaitEvent(NULL);
         continue;
      }

      // render the scene
      {
         int start = SDL_GetTicks();

         OnRender();

         int time = SDL_GetTicks() - start;
         if (time < 0)
            continue; // if time is negative, the time probably overflew, so continue asap

         const int millisecondsForFrame = int(1000.0 / m_framesPerSecond);

         int sleepTime = millisecondsForFrame - time;
         if (sleepTime > 0)
            SDL_Delay(sleepTime);

         fc.OnNextFrame();
      }

      // count ticks for fps
      double dFramesPerSecond;
      if (m_bUpdateFrameCount && fc.GetUpdatedCount(dFramesPerSecond))
         UpdateCaption(dFramesPerSecond);
   }
}

void MainGameLoop::UpdateCaption(double dFramesPerSecond)
{
   // set new caption
   CString cszCaption;
   cszCaption.Format(_T("%s; %3u.%01u frames/s"),
      m_cszClientName.GetString(),
      unsigned(dFramesPerSecond),
      unsigned((dFramesPerSecond-unsigned(dFramesPerSecond))*10.0));

   UpdateCaption(cszCaption);
}

void MainGameLoop::ProcessEvents()
{
   SDL_Event evt;

   // get another event
   while (SDL_PollEvent(&evt) && !m_bExitLoop)
   {
      switch (evt.type)
      {
      case SDL_QUIT:
         // handle quit requests
         m_bExitLoop = true;
         break;

      case SDL_APP_WILLENTERBACKGROUND:
         m_bAppActive = false;
         break;

      case SDL_APP_DIDENTERFOREGROUND:
         m_bAppActive = true;
         break;

      case SDL_WINDOWEVENT:
         switch (evt.window.event)
         {
         case SDL_WINDOWEVENT_ENTER:
            m_bAppActive = true;
            break;

         case SDL_WINDOWEVENT_LEAVE:
            m_bAppActive = false;
            break;

         default:
            break;
         }
         break;

      default:
         OnEvent(evt);
         break;
      }
   }
}

void MainGameLoop::QuitLoop()
{
   m_bExitLoop = true;
}
