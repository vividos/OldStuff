//
// MultiplayerOnlineGame - multiplayer game project
// Copyright (C) 2008-2016 Michael Fink
//
/// \file RenderWindow2D.cpp 2D render window with scaler
//

// includes
#include "StdAfx.h"
#include "RenderWindow2D.hpp"
#include "PalettedSurface.hpp"
#include "ScalerLib.hpp"
#include <map>

/// mapping from scaler name to dll name
static std::map<CString, CString> scalerToDllMap =
{
   { _T("Scale2x"), _T("Scale2x.dll") }
};

RenderWindow2D::RenderWindow2D(unsigned int width, unsigned int height, bool fullscreen, const CString& scaler, unsigned int requestedScaleFactor)
   :scalerFunc(nullptr),
   scaleFactor(1),
   unscaledWidth(width),
   unscaledHeight(height)
{
   LoadScaler(scaler, requestedScaleFactor);

   scaleFactor = requestedScaleFactor;

   if (!CreateWindowAndRenderer(width, height))
      throw std::runtime_error("couldn't create window");

   SetFullscreen(fullscreen);

   SDL_SetRenderDrawColor(renderer.get(), 0, 0, 0, 255); // black
   SDL_RenderClear(renderer.get());
   SDL_RenderPresent(renderer.get());
}

RenderWindow2D::~RenderWindow2D() throw()
{
}

unsigned int RenderWindow2D::BitsPerPixel() const
{
   ATLASSERT(surface != nullptr); // only valid after window was created

   return surface->format->BitsPerPixel;
}

bool RenderWindow2D::IsFullscreen() const
{
   if (window == nullptr)
      return false;

   Uint32 windowFlags = SDL_GetWindowFlags(window.get());

   return (windowFlags & SDL_WINDOW_FULLSCREEN_DESKTOP) != 0;
}

void RenderWindow2D::SetFullscreen(bool fullscreen)
{
   if (window == nullptr)
      return;

   if (SDL_SetWindowFullscreen(window.get(), fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0) < 0)
   {
      ATLTRACE(_T("error setting fullscreen mode %hs\n"), SDL_GetError());
   }

   if (!fullscreen)
   {
      SDL_SetWindowSize(window.get(), unscaledWidth * scaleFactor, unscaledHeight * scaleFactor);
   }
}

void RenderWindow2D::SetCaption(LPCTSTR caption)
{
   ATLASSERT(window != nullptr);

   SDL_SetWindowTitle(window.get(), CStringA(caption));
}

void RenderWindow2D::Blit(unsigned int destX, unsigned int destY, const PalettedSurface& buffer)
{
   if (BitsPerPixel() == 32)
      Blit32(destX, destY, buffer);
   else
      Blit16(destX, destY, buffer);
}

void RenderWindow2D::Blit32(unsigned int destX, unsigned int destY, const PalettedSurface& buffer)
{
   const Uint8* sourceIndices = buffer.Data();

   const Uint32* palette = buffer.GetPalette().data();

   Uint32* destPixels = reinterpret_cast<Uint32*>(unscaledSurfaceBuffer.data());

   destPixels += destY * surface->w + destX;

   unsigned int maxX = buffer.Width();
   unsigned int maxY = buffer.Height();

   unsigned int sourcePitch = maxX;

   // clip when needed
   if (destX + maxX > unsigned(surface->w))
      maxX = unsigned(surface->w) - destX;

   if (destY + maxY > unsigned(surface->h))
      maxY = unsigned(surface->h) - destY;

   // line by line, then column by column
   for (unsigned int currentY = 0; currentY < maxY; currentY++)
   {
      for (unsigned int currentX = 0; currentX < maxX; currentX++)
      {
         Uint8 index = sourceIndices[currentX];
         destPixels[currentX] = palette[index];
      }

      sourceIndices += sourcePitch;
      destPixels += unscaledWidth;
   }
}

void RenderWindow2D::Blit16(unsigned int destX, unsigned int destY, const PalettedSurface& buffer)
{
   const Uint8* sourceIndices = buffer.Data();

   const Uint32* palette = buffer.GetPalette().data();

   Uint16* destPixels = reinterpret_cast<Uint16*>(unscaledSurfaceBuffer.data());

   destPixels += destY * surface->w + destX;

   unsigned int maxX = buffer.Width();
   unsigned int maxY = buffer.Height();

   unsigned int sourcePitch = maxX;

   // clip when needed
   if (destX + maxX > unsigned(surface->w))
      maxX = unsigned(surface->w) - destX;

   if (destY + maxY > unsigned(surface->h))
      maxY = unsigned(surface->h) - destY;

   // line by line, then column by column
   for (unsigned int currentY = 0; currentY < maxY; currentY++)
   {
      for (unsigned int currentX = 0; currentX < maxX; currentX++)
      {
         Uint8 index = sourceIndices[currentX];
         destPixels[currentX] = static_cast<Uint16>(palette[index] & 0xffff);
      }

      sourceIndices += sourcePitch;
      destPixels += unscaledWidth;
   }
}

void RenderWindow2D::Update()
{
   ATLASSERT(texture != nullptr);
   ATLASSERT(surface != nullptr);
   ATLASSERT(renderer != nullptr);

   if (scalerFunc != nullptr)
   {
      // scale then update
      unsigned int bufferType = surface->format->BitsPerPixel == 32 ? 888 : 565;

      scalerFunc(bufferType,
         scaleFactor,
         reinterpret_cast<Uint8*>(surface->pixels),
         unscaledSurfaceBuffer.data(),
         unscaledWidth, unscaledHeight);

      SDL_UpdateTexture(texture.get(), nullptr, surface->pixels, surface->pitch);
   }
   else
   {
      // just update from unscaled surface
      SDL_UpdateTexture(texture.get(), nullptr, unscaledSurfaceBuffer.data(), unscaledWidth * BitsPerPixel() / 8);
   }

   SDL_RenderClear(renderer.get());
   SDL_RenderCopy(renderer.get(), texture.get(), nullptr, nullptr);
   SDL_RenderPresent(renderer.get());
}

/// \param[in] scaler scaler name
/// \param[in,out] requestedScaleFactor scale factor that should be used. may
/// be adjusted according to what scale factors the scaler supports.
void RenderWindow2D::LoadScaler(const CString& scaler, unsigned int& requestedScaleFactor)
{
   if (requestedScaleFactor == 0)
      requestedScaleFactor = 1;

   if (requestedScaleFactor == 1)
      return;

   if (scalerToDllMap.find(scaler) != scalerToDllMap.end())
   {
      scalerLib = std::make_shared<ScalerLib>(scalerToDllMap[scaler]);
      if (scalerLib->Valid())
      {
         scalerFunc = scalerLib->GetScalerFunc();
      }
   }

   if (scalerFunc == nullptr)
   {
      scalerFunc = &RenderWindow2D::PointScale;
   }
}

void RenderWindow2D::PointScale(
   unsigned int bufferType,
   unsigned int scaleFactor,
   unsigned char* destBuffer,
   const unsigned char* sourceBuffer,
   unsigned int unscaledWidth,
   unsigned int unscaledHeight) throw()
{
   unsigned int bytesPerPixel = bufferType == 888 || bufferType == 32 ? 4 : 2;

   if (scaleFactor == 1)
   {
      memcpy(destBuffer, sourceBuffer, unscaledWidth * unscaledHeight * bytesPerPixel);
      return;
   }

   for (unsigned int currentY = 0, maxY = unscaledHeight; currentY < maxY; currentY++)
   {
      for (unsigned int line = 0; line < scaleFactor; line++)
      {
         unsigned int destY = currentY * scaleFactor + line;

         for (unsigned int currentX = 0, maxX = unscaledWidth; currentX < maxX; currentX++)
         {
            for (unsigned int column = 0; column < scaleFactor; column++)
            {
               unsigned int destX = currentX * scaleFactor + column;

               unsigned int destOffset = (destY * unscaledWidth * scaleFactor + destX)  * bytesPerPixel;
               unsigned int srcOffset = (currentY * unscaledWidth + currentX) * bytesPerPixel;

               for (unsigned int byte = 0; byte < bytesPerPixel; byte++)
               {
                  destBuffer[destOffset + byte] = sourceBuffer[srcOffset + byte];
               }
            }
         }
      }
   }
}

bool RenderWindow2D::CreateWindowAndRenderer(unsigned int width, unsigned int height)
{
   int iRet = SDL_Init(SDL_INIT_VIDEO);
   if (iRet < 0)
   {
      ATLTRACE(_T("couldn't init SDL\n"));
      return false;
   }

   unsigned int scaledWidth = width * scaleFactor;
   unsigned int scaledHeight = height * scaleFactor;

   SDL_DisplayMode mode = {0};
   if (SDL_GetDisplayMode(0, 0, &mode) < 0)
   {
      return false;
   }

   int bitsPerPixel = SDL_BITSPERPIXEL(mode.format) == 24 ? 32 : SDL_BITSPERPIXEL(mode.format);

   unscaledSurfaceBuffer.resize(width * height * (bitsPerPixel / 8), 0);

   SDL_Window* rawWindow = nullptr;
   SDL_Renderer* rawRenderer = nullptr;
   SDL_CreateWindowAndRenderer(
      scaledWidth,
      scaledHeight,
      SDL_WINDOW_SHOWN | SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC,
      &rawWindow, &rawRenderer);

   if (rawWindow == nullptr || rawRenderer == nullptr)
      return false;

   window.reset(rawWindow, &SDL_DestroyWindow);
   renderer.reset(rawRenderer, &SDL_DestroyRenderer);

   SDL_RenderSetLogicalSize(renderer.get(), scaledWidth, scaledHeight);

   ATLTRACE(_T("SDL window, res=%ux%u, %i bpp, refresh rate: %i Hz\n"),
      scaledWidth,
      scaledHeight,
      bitsPerPixel,
      mode.refresh_rate);

   // create texture
   Uint32 pixelFormat = bitsPerPixel == 16 ? SDL_PIXELFORMAT_RGB565 : SDL_PIXELFORMAT_ARGB8888;

   int bpp = 0;
   Uint32 rmask = 0, gmask = 0, bmask = 0, amask = 0;
   SDL_PixelFormatEnumToMasks(pixelFormat, &bpp, &rmask, &gmask, &bmask, &amask);

   SDL_Surface* rawSurface = SDL_CreateRGBSurface(0, scaledWidth, scaledHeight, bpp,
      rmask, gmask, bmask, amask);

   if (rawSurface == nullptr)
      return false;

   surface.reset(rawSurface, &SDL_FreeSurface);

   SDL_Texture* rawTexture = SDL_CreateTexture(
      renderer.get(),
      pixelFormat,
      SDL_TEXTUREACCESS_STREAMING,
      scaledWidth,
      scaledHeight);

   if (rawTexture != nullptr)
      texture.reset(rawTexture, &SDL_DestroyTexture);

   return rawTexture != nullptr;
}
