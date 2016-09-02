//
// MultiplayerOnlineGame - multiplayer game project
// Copyright (C) 2008-2016 Michael Fink
//
/// \file PalettedSurface.cpp Surface that has an 8-bit palette
//

// includes
#include "stdafx.h"
#include "PalettedSurface.hpp"

PalettedSurface::PalettedSurface(unsigned int xres, unsigned int yres, unsigned int bitsPerPixel_)
   :width(xres),
   height(yres),
   bitsPerPixel(bitsPerPixel_)
{
   std::uninitialized_fill(palette.begin(), palette.end(), 0);

   surfaceBuffer.resize(xres * yres, 0);
}

void PalettedSurface::Clear(Uint8 color)
{
   memset(surfaceBuffer.data(), color, surfaceBuffer.size());
}

void PalettedSurface::SetPalette(Uint8* rawPalette, size_t lengthInEntries)
{
   ATLASSERT(lengthInEntries < 256);

   // ABGR
   unsigned int shiftR = 0;
   unsigned int shiftG = 8;
   unsigned int shiftB = 16;

   unsigned int maskR = 0x0000ff;
   unsigned int maskG = 0x00ff00;
   unsigned int maskB = 0xff0000;

   if (bitsPerPixel == 16)
   {
      shiftR = 5 + 6;
      shiftG = 6;
      shiftB = 0;

      maskR = 0xf800;
      maskG = 0x07e0;
      maskB = 0x001f;
   }

   for (size_t index = 0; index < lengthInEntries && lengthInEntries < palette.size(); index++)
   {
      Uint32 red = rawPalette[0];
      Uint32 green = rawPalette[1];
      Uint32 blue = rawPalette[2];

      palette[index] =
         ((red << shiftR) & maskR) |
         ((green << shiftG) & maskG) |
         ((blue << shiftB) & maskB);

      rawPalette += 3;
   }
}
