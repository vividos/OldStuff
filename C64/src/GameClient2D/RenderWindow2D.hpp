//
// MultiplayerOnlineGame - multiplayer game project
// Copyright (C) 2008-2016 Michael Fink
//
/// \file RenderWindow2D.hpp 2D render window with scaler
//
#pragma once

// includes
#include <memory>
#include <array>
#include <vector>
#include <SDL.h>

// forward references
class ScalerLib;
class PalettedSurface;

/// Render window for 2D applications
class RenderWindow2D
{
public:
   /// ctor; creates a new render window
   RenderWindow2D(unsigned int width, unsigned int height, bool fullscreen,
      const CString& scaler, unsigned int requestedScaleFactor);
   /// dtor
   ~RenderWindow2D() throw();

   /// returns number of bits per pixel of window
   unsigned int BitsPerPixel() const;

   /// returns if window is currently displayed in fullscreen mode
   bool IsFullscreen() const;

   /// sets window fullscreen or windowed
   void SetFullscreen(bool fullscreen);

   /// sets new window caption
   void SetCaption(LPCTSTR caption);

   /// blits paletted surface buffer to window
   void Blit(unsigned int destX, unsigned int destY, const PalettedSurface& buffer);

   /// updates window using all surface operations done so far
   void Update();

private:
   /// loads scaler if available
   void LoadScaler(const CString& scaler, unsigned int& scaleFactor);

   /// scale function typedef
   typedef void(*T_fnScale)(
      unsigned int bufferType, // one of 16, 555, 565, 32 or 888
      unsigned int scaleFactor, // scale factor, either 2, 3 or 4
      unsigned char* destBuffer,
      const unsigned char* sourceBuffer,
      unsigned int unscaledWidth,
      unsigned int unscaledHeight);

   /// point scale scaler function
   static void PointScale(unsigned int bufferType,
      unsigned int scaleFactor,
      unsigned char* destBuffer,
      const unsigned char* sourceBuffer,
      unsigned int width,
      unsigned int height) throw();

   /// creates SDL window and renderer
   bool CreateWindowAndRenderer(unsigned int width, unsigned int height);

   /// blits paletted surface buffer to 32-bit window
   void Blit32(unsigned int destX, unsigned int destY, const PalettedSurface& buffer);

   /// blits paletted surface buffer to 16-bit window
   void Blit16(unsigned int destX, unsigned int destY, const PalettedSurface& buffer);

private:
   std::shared_ptr<SDL_Window> window;       ///< SDL window
   std::shared_ptr<SDL_Renderer> renderer;   ///< SDL renderer
   std::shared_ptr<SDL_Surface> surface;     ///< SDL surface
   std::shared_ptr<SDL_Texture> texture;     ///< SDL texture

   /// scaler library, if loaded
   std::shared_ptr<ScalerLib> scalerLib;

   /// scale factor for scaler
   unsigned int scaleFactor;

   /// unscaled width of window
   unsigned int unscaledWidth;

   /// unscaled height of window
   unsigned int unscaledHeight;

   /// scaler function; may be nullptr, when no scaling is necessary
   T_fnScale scalerFunc;

   /// surface buffer for unscaled image
   std::vector<Uint8> unscaledSurfaceBuffer;
};
