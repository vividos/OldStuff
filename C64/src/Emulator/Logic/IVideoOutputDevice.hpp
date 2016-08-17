//
// Emulator - Simple C64 emulator
// Copyright (C) 2003-2016 Michael Fink
//
/// \file IVideoOutputDevice.hpp Memory access interface
//
#pragma once

// includes

namespace C64
{

/// Interface to video output device
class IVideoOutputDevice
{
public:
   /// dtor
   virtual ~IVideoOutputDevice() throw()
   {
   }

   /// outputs a whole video scanline
   virtual void OutputLine(WORD wRasterline, BYTE abLine[0x0200]) = 0;

   /// notifies when outputting a whole screen has been finished
   virtual void ScreenCompleted() = 0;
};

} // namespace C64
