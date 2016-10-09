//
// c64intro - C64 Intro using SDL
// Copyright (C) 2003-2016 Michael Fink
//
/// \file Intro.cpp Intro main class
//

// includes
#include "StdAfx.h"
#include "Intro.hpp"
#include "RenderWindow2D.hpp"
#include "PalettedSurface.hpp"
#include <cmath>

#pragma warning(disable: 28159) // Consider using 'GetTickCount64' instead of 'GetTickCount'

/// x resolution of window
const unsigned int xres = 320;

/// y resolution of window
const unsigned int yres = 200;

/// scroll speed of text scroller, in characters per second
const double c_scrollCharsPerSecond = 2.0;

/// moving time for big raster bar, in seconds
const double c_bigRasterBarMovingTime = 1.0;

/// display time for big raster bar, in seconds
const double c_bigRasterBarDisplayTime = 3.0;

/// C64 palette
std::array<Uint8, 16 * 3> Intro::palette =
{
   0x00, 0x00, 0x00, // 0 black
   0xff, 0xff, 0xff, // 1 white
   0x88, 0x00, 0x00, // 2 red
   0xaa, 0xff, 0xee, // 3 cyan
   0xcc, 0x44, 0xcc, // 4 pink
   0x00, 0xcc, 0x55, // 5 green
   0x00, 0x00, 0xaa, // 6 blue
   0xee, 0xee, 0x77, // 7 yellow
   0xdd, 0x88, 0x55, // 8 orange
   0x66, 0x44, 0x00, // 9 brown
   0xff, 0x77, 0x77, // 10 light red
   0x33, 0x33, 0x33, // 11 dark gray
   0x77, 0x77, 0x77, // 12 medium gray
   0xaa, 0xff, 0x66, // 13 light green
   0x00, 0x88, 0xff, // 14 light blue
   0xbb, 0xbb, 0xbb  // 15 light gray
};

/// all bar colors that can be used for the big raster bar
std::vector<std::vector<Uint8>> allBarColors =
{
   // top/bottom mirrored colors
   { 6, 14, 3, 1, 3, 14, 6 }, // blue, l.blue, cyan
   { 6, 14, 5, 1, 5, 14, 6 }, // blue, l.blue, green
   { 6, 5, 3, 1, 3, 5, 6 }, // blue, green, cyan
   { 2, 4, 15, 1, 15, 4, 2 }, // red, purple, l.gray
   { 9, 8, 7, 1, 7, 8, 9 }, // brown, orange, yellow
   { 9, 10, 7, 1, 7, 10, 9 }, // brown, l.red, yellow
   { 6, 5, 7, 1, 7, 5, 6 }, // blue, green, yellow
   { 11, 12, 15, 1, 15, 12, 11 }, // d.gray, m.gray, l.gray
   { 2, 10, 7, 1, 7, 10, 2 }, // red, l.red, yellow
   { 2, 8, 7, 1, 7, 8, 2 }, // red, orange, yellow

   // top/bottom mixed colors
   { 6, 5, 7, 1, 3, 14, 6 }, // blue, green, yellow, white, cyan, l.blue, blue
   { 6, 5, 7, 1, 3, 5, 6 }, // blue, green, yellow, white, cyan, green, blue
   { 6, 5, 7, 1, 7, 8, 9 }, // blue, green, yellow, white, yellow, orange, brown
   { 6, 5, 3, 1, 7, 8, 9 }, // blue, green, cyan, white, yellow, orange, brown
   { 6, 14, 5, 1, 7, 8, 9 }, // blue, l.blue, green, white, yellow, orange, brown
   { 6, 14, 5, 1, 7, 10, 9 }, // blue, l.blue, green, white, yellow, l.red, brown
};

Intro::Intro()
   :MainGameLoop(true, _T("c64intro")),
#ifdef _WIN32
   window(new RenderWindow2D(xres, yres, false, _T("Scale2x"), 2)),
#elif defined(__ANDROID__)
   window(new RenderWindow2D(xres, yres, true, _T(""), 1)),
#endif
   buffer(new PalettedSurface(xres, yres, window->BitsPerPixel())),
   currentBigRasterBarState(movingOut)
{
   buffer->SetPalette<16>(palette);

   startTicks = SDL_GetTicks();

   // triggers a new bar at first tick
   bigRasterBarStartTicks = startTicks + Uint32(c_bigRasterBarMovingTime * 1000.0 + 100);

   scrollerPos = 0.0;

   scrollerText = "Hello World!";

   scrollerText.insert(0, 41, ' ');
   scrollerText.append(41, ' ');

   OnTick();
}

Intro::~Intro() throw()
{
}

void Intro::Run()
{
   MainGameLoop::Run();
}

void Intro::UpdateCaption(const CString& caption)
{
   window->SetCaption(caption);
}

/// macro to convert from degrees to radians
#define deg2rad(deg) ((deg)/180.0*3.1415)

void Intro::OnTick()
{
   Uint32 dwCurrentTicks = SDL_GetTicks() - startTicks;
   double elapsed = dwCurrentTicks / 1000.0;

   CalcMovingBars(elapsed);
   CalcScrollerBar(elapsed);
   CalcBigRasterBar();
}

void Intro::CalcMovingBars(double elapsed)
{
   const float movingBarStart = 70.0;
   const float movingBarEnd = 100.0;
   const float movingBarWidth = movingBarEnd - movingBarStart;
   const float barHeight = 30.0;

   float amplitudeBlue = 20.0;
   rasterBarPosBlue = int(movingBarStart + amplitudeBlue * std::sin(deg2rad(elapsed / 2.0 * 360.0)));

   float meanGreen = movingBarEnd - barHeight;
   float amplitudeGreen = 20.0;
   rasterBarPosGreen = int(meanGreen + amplitudeGreen * -abs(std::sin(deg2rad((elapsed + 0.5) / 1.5 * 360.0))));

   float meanRed = movingBarStart + movingBarWidth / 2.0;
   float amplitudeRed = movingBarWidth / 2.0;
   rasterBarPosRed = int(meanRed + amplitudeRed * std::sin(deg2rad(elapsed / 4.0 * 360.0)));

   rasterBarRedForeground = std::cos(deg2rad(elapsed / 4.0 * 360.0)) > 0;
}

void Intro::CalcScrollerBar(double elapsed)
{
   rasterBarPosScroller = int(135.0 + 20.0 * -abs(std::sin(deg2rad(elapsed / 4.0 * 360.0))));

   scrollerPos = elapsed * c_scrollCharsPerSecond;

   if (!scrollerText.empty() && size_t(scrollerPos) > scrollerText.size())
   {
      scrollerPos = std::fmod(scrollerPos, double(scrollerText.size()));
   }
}

void Intro::CalcBigRasterBar()
{
   Uint32 dwBigRasterBarCurrentTicks = SDL_GetTicks() - bigRasterBarStartTicks;
   double bigRasterBarElapsed = dwBigRasterBarCurrentTicks / 1000.0;

   switch (currentBigRasterBarState)
   {
   case movingIn:
      if (bigRasterBarElapsed >= c_bigRasterBarMovingTime)
      {
         ATLTRACE(_T("changing to bar state DISPLAY\n"));

         currentBigRasterBarState = display;
         bigRasterBarStartTicks = SDL_GetTicks();
      }

      rasterBarPosBig =
         int(162.0 + 37.0 - 37.0 * std::sin(deg2rad(bigRasterBarElapsed / c_bigRasterBarMovingTime / 4.0 * 360.0)));
      break;

   case display:
      rasterBarPosBig = 162;
      if (bigRasterBarElapsed > c_bigRasterBarDisplayTime)
      {
         ATLTRACE(_T("changing to bar state MOVING-OUT\n"));

         currentBigRasterBarState = movingOut;
         bigRasterBarStartTicks = SDL_GetTicks();
      }
      break;

   case movingOut:
      if (bigRasterBarElapsed >= c_bigRasterBarMovingTime)
      {
         ATLTRACE(_T("changing to bar state MOVING-IN\n"));

         currentBigRasterBarState = movingIn;
         bigRasterBarStartTicks = SDL_GetTicks();

         bigRasterBarIndex = size_t(double(rand() * allBarColors.size()) / RAND_MAX);
      }

      rasterBarPosBig =
         int(162.0 + 37.0 - 37.0 * std::sin(deg2rad(bigRasterBarElapsed / c_bigRasterBarMovingTime / 4.0 * 360.0 + 90.0)));
      break;

   default:
      ATLASSERT(false);
      break;
   }
}

/// generates a raster bar from given "base" bar colors by interleaving previous with current colors
void GenerateRasterBar(const std::vector<Uint8>& barColors, std::vector<Uint8>& rasterBar)
{
   rasterBar.push_back(0); // black
   Uint8 prevColor = 0;

   size_t midpoint = barColors.size() / 2;
   for (size_t colorIndex = 0; colorIndex < barColors.size(); colorIndex++)
   {
      Uint8 color = barColors[colorIndex];

      rasterBar.push_back(color);
      rasterBar.push_back(prevColor);
      rasterBar.push_back(color);
      rasterBar.push_back(color);

      if (colorIndex == midpoint)
         rasterBar.push_back(color);

      prevColor = color;
   }

   rasterBar.push_back(0); // black
   rasterBar.push_back(prevColor);
   rasterBar.push_back(0); // black
}

void Intro::OnRender()
{
   buffer->Clear(0); // black

   // render moving bars
   RenderMovingBars();
   RenderScrollerBar();
   RenderBigRasterBar();

   // blit screen pixels
   window->Blit(0, 0, *buffer);

   window->Update();
}

void Intro::RenderMovingBars()
{
   Uint8 rasterBarBlue[] =
   {
      0, 6, 14, 4, 1, 1, 1, 1, 4, 14, 6, 0,
   };

   Uint8 rasterBarGreen[] =
   {
      0, 5, 13, 3, 1, 1, 1, 1, 3, 13, 5, 0,
   };

   Uint8 rasterBarRed[] =
   {
      0, 2, 10, 8, 7, 1, 1, 1, 1, 7, 8, 10, 2, 0,
   };

   if (rasterBarRedForeground)
      DrawRasterBars(rasterBarRed, SDL_TABLESIZE(rasterBarRed), rasterBarPosRed);

   DrawRasterBars(rasterBarBlue, SDL_TABLESIZE(rasterBarBlue), rasterBarPosBlue);
   DrawRasterBars(rasterBarGreen, SDL_TABLESIZE(rasterBarGreen), rasterBarPosGreen);

   if (!rasterBarRedForeground)
      DrawRasterBars(rasterBarRed, SDL_TABLESIZE(rasterBarRed), rasterBarPosRed);
}

void Intro::RenderScrollerBar()
{
   Uint8 rasterBarScroller[] =
   {
      0,
      // blue, purple, l.blue, l.green, yellow
      6, 4, 14, 13, 7,
      //2, 10, 8, 7,
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
      // yellow, l.red, orange, red, brown
      7, 10, 8, 2, 9,
      0,
   };

   DrawRasterBars(rasterBarScroller, SDL_TABLESIZE(rasterBarScroller), rasterBarPosScroller);

   // TODO draw text
   size_t pos = size_t(scrollerPos);

   size_t fractOffset = size_t((scrollerPos - pos) * 8.0);

   std::string text = scrollerText.substr(pos, 41);

   static size_t s_pos = 123451;
   if (s_pos != pos)
   {
      s_pos = pos;
      ATLTRACE(_T("SCROLL: %hs\n"), text.c_str());
   }
}

void Intro::RenderBigRasterBar()
{
   std::vector<Uint8> rasterBarBig;
   GenerateRasterBar(allBarColors[bigRasterBarIndex], rasterBarBig);

   DrawRasterBars(rasterBarBig.data(), rasterBarBig.size(), rasterBarPosBig);
}

void Intro::OnEvent(SDL_Event& evt)
{
   switch (evt.type)
   {
   case SDL_KEYDOWN:
      switch (evt.key.keysym.sym)
      {
      case SDLK_RETURN:
      case SDLK_SPACE:
         if (evt.key.keysym.sym == SDLK_RETURN &&
            (evt.key.keysym.mod & KMOD_ALT) != 0)
         {
            window->SetFullscreen(!window->IsFullscreen());
         }
         else
            QuitLoop();
         break;

      default:
         break;
      }
      break;

   case SDL_QUIT:
      QuitLoop();
      break;
   }
}

void Intro::DrawRasterBars(Uint8* raster, unsigned int size, unsigned int ypos)
{
   for (unsigned int i = 0; i < size; i++)
   {
      if (ypos + i >= yres)
         break;

      Uint8* rawBuffer = buffer->Data();

      memset(&rawBuffer[(ypos + i) * xres], raster[i], xres);
   }
}

#ifdef __ANDROID__
extern "C" void Java_org_libsdl_app_SDLActivity_nativeInit();
#endif

int main(int argc, char* argv[])
{
#ifdef __ANDROID__
   void* p = (void*)&Java_org_libsdl_app_SDLActivity_nativeInit;
#endif

   argc; argv;

   Intro intro;
   intro.Run();

   return 0;
}
