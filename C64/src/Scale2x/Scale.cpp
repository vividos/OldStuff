//
// Scale2x - Adapter for Scale2x scaler for RenderWindows2D
// Copyright (C) 2016 Michael Fink
//
/// \file Scale.cpp Scale function
//

// includes
extern "C"
{
#include "scale2x-3.2\scalebit.h"
}

/// scales buffer with given scale factor
extern "C"
__declspec(dllexport)
void Scale(
   unsigned int bufferType,
   unsigned int scaleFactor,
   unsigned char* destBuffer,
   const unsigned char* sourceBuffer,
   unsigned int unscaledWidth,
   unsigned int unscaledHeight) throw()
{
   int bytesPerPixel = bufferType == 888 || bufferType == 32 ? 4 : 2;

#ifdef _DEBUG
   if (0 != ::scale_precondition(scaleFactor, bytesPerPixel, unscaledWidth, unscaledHeight))
      return;
#endif

   ::scale(
      scaleFactor,
      destBuffer,
      unscaledWidth * scaleFactor * bytesPerPixel,
      sourceBuffer,
      unscaledWidth * bytesPerPixel,
      bytesPerPixel,
      unscaledWidth,
      unscaledHeight);
}
