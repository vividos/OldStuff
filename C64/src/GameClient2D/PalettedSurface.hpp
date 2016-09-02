//
// MultiplayerOnlineGame - multiplayer game project
// Copyright (C) 2008-2016 Michael Fink
//
/// \file PalettedSurface.hpp Surface that has an 8-bit palette
//
#pragma once

// includes
#include <array>
#include <vector>
#include <algorithm>

/// Surface that uses an 8-bit RGB palette
class PalettedSurface
{
public:
   /// ctor; creates new surface
   PalettedSurface(unsigned int xres, unsigned int yres, unsigned int bitsPerPixel);

   /// returns width of surface
   unsigned int Width() const { return width; }

   /// returns height of surface
   unsigned int Height() const { return height; }

   /// returns surface buffer; non-const version
   Uint8* Data() { return surfaceBuffer.data(); }

   /// returns surface buffer; const version
   const Uint8* Data() const { return surfaceBuffer.data(); }

   /// sets new palette with N color indices
   /// \tparam N number of palette indices
   template <int N>
   void SetPalette(std::array<Uint8, N * 3> paletteToSet)
   {
      static_assert(N < 256, "palette must have at most 256 entries");

      SetPalette(paletteToSet.data(), paletteToSet.size() / 3);
   }

   /// returns currently set palette
   const std::array<Uint32, 256>& GetPalette() const throw() { return palette; }

   /// clears surface with given color index
   void Clear(Uint8 color);

private:
   /// sets palette
   void SetPalette(Uint8* rawPalette, size_t lengthInEntries);

private:
   /// surface height
   unsigned int height;

   /// surface width
   unsigned int width;

   /// bits per pixel of target surface
   unsigned int bitsPerPixel;

   /// surface buffer containing indices into palette
   std::vector<Uint8> surfaceBuffer;

   /// RGBA palette values for 8-bit indexed images
   std::array<Uint32, 256> palette;
};
