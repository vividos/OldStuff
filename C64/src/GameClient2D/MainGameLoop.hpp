//
// MultiplayerOnlineGame - multiplayer game project
// Copyright (C) 2008-2016 Michael Fink
//
/// \file MainGameLoop.hpp Main game loop
//
#pragma once

// includes
#include <SDL.h>
#include <atomic>

// forward references

/// main game loop
class MainGameLoop
{
public:
   /// ctor
   MainGameLoop(bool bUpdateFrameCount = false,
      const CString& cszClientName = _T(""));

   /// runs main game loop
   void Run();

   /// quits loop (from another thread)
   void QuitLoop();

   // new virtual methods

   /// called to update caption
   virtual void UpdateCaption(const CString& caption) { caption; }

   /// processes game logic
   virtual void OnTick() {}

   /// renders screen
   virtual void OnRender() {}

   /// called on new event
   virtual void OnEvent(SDL_Event& evt) { evt; }

private:
   /// updates window caption
   void UpdateCaption(double dFramesPerSecond);

   /// called to process events
   void ProcessEvents();

private:
   /// indicates that loop should exit
   std::atomic<bool> m_bExitLoop;

   /// indicates if frame count should be updated in window caption
   bool m_bUpdateFrameCount;

   /// client name; used for window caption
   CString m_cszClientName;

   /// indicates if application is active or hidden
   bool m_bAppActive;

   /// indicates if app window has mouse focus
   bool m_bMouseFocus;

   /// indicates if app window has keyboard input focus
   bool m_bInputFocus;
};
