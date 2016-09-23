//
// c64intro - C64 Intro using SDL
// Copyright (C) 2003-2016 Michael Fink
//
/// \file Intro.hpp Intro main class
//
#pragma once

// includes
#include <array>
#include <vector>
#include <memory>
#include "MainGameLoop.hpp"

// forward references
class RenderWindow2D;
class PalettedSurface;

/// a C64 intro
class Intro : private MainGameLoop
{
public:
   /// ctor
   Intro();
   /// dtor
   virtual ~Intro() throw();

   /// runs intro until user presses Space or Enter key
   void Run();

private:
   /// called to update caption
   virtual void UpdateCaption(const CString& caption) override;

   /// processes game logic
   virtual void OnTick() override;

   /// renders screen
   virtual void OnRender() override;

   /// called on new event
   virtual void OnEvent(SDL_Event& evt) override;

   /// calculates positions of moving bars
   void CalcMovingBars(double elapsed);

   /// calculates position of scroller bar and text
   void CalcScrollerBar(double elapsed);

   /// calculates position of big raster bar
   void CalcBigRasterBar();

   /// draws a raster bar on specified Y position
   void DrawRasterBars(Uint8* raster, unsigned int size, unsigned int ypos);

   /// renders moving bars
   void RenderMovingBars();

   /// renders scroller bar
   void RenderScrollerBar();

   // renders big raster bar
   void RenderBigRasterBar();

private:
   /// window
   std::unique_ptr<RenderWindow2D> window;

   /// surface buffer
   std::unique_ptr<PalettedSurface> buffer;

   /// default C64 palette
   static std::array<Uint8, 16 * 3> palette;

   // model

   /// tick count of start of intro
   DWORD dwStartTicks;

   // top part: moving logo

   // middle part: moving raster bars

   unsigned int rasterBarPosBlue;   ///< position of blue raster bar
   unsigned int rasterBarPosGreen;  ///< position of green raster bar
   unsigned int rasterBarPosRed;    ///< position of red raster bar
   bool rasterBarRedForeground;     ///< foreground flag for red raster bar

   // bottom upper part: raster bar with scroller

   /// complete scroller text
   std::string scrollerText;

   /// current scroller position, with fractional values
   double scrollerPos;

   /// position of scroller raster bar
   unsigned int rasterBarPosScroller;

   // bottom lower part: big raster bar with changing colors

   /// current position of big raster bar
   unsigned int rasterBarPosBig;

   /// index of current big raster bar
   size_t bigRasterBarIndex;

   /// state of big raster bar
   enum BigRasterBarState
   {
      movingIn,   ///< raster bar moves in from below
      display,    ///< raster bar is displayed for some time
      movingOut,  ///< raster bar moves out
   };

   /// current state of big raster bar
   BigRasterBarState currentBigRasterBarState;

   /// start tick count for current state of big raster bar
   DWORD dwBigRasterBarStartTicks;
};
