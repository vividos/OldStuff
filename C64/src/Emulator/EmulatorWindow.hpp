//
// c64emu - C64 Emulator using SDL
// Copyright (C) 2006-2016 Michael Fink
//
/// \file EmulatorWindow.hpp Emulator window
//
#pragma once

// includes
#include "Machine.hpp"
#include "IVideoOutputDevice.hpp"
#include "MainGameLoop.hpp"

// forward references
class RenderWindow2D;
class PalettedSurface;

/// main emulator window
class EmulatorWindow:
   public C64::IVideoOutputDevice,
   public MainGameLoop
{
public:
   /// ctor
   EmulatorWindow(C64::Machine& emulator);
   /// dtor
   ~EmulatorWindow();

   /// loads .t64 or .p00 file into memory
   void Load(LPCTSTR filename);

   /// runs loaded C64 program
   void Run();

private:
   /// finds C64 basic SYS command and returns address, or 0 when not found
   WORD FindBasicSysCommand(WORD startAddress);

   /// initializes SDL
   void InitSDL();

   // virtual methods from IVideoOutputDevice
   virtual void OutputLine(WORD wRasterline, BYTE abLine[0x0200]);
   virtual void ScreenCompleted();

   // virtual methods from MainGameLoop

   /// called to update caption
   virtual void UpdateCaption(const CString& caption) override;

   /// renders screen
   virtual void OnRender() override;

   /// called on new event
   virtual void OnEvent(SDL_Event& evt) override;

private:
   /// C64 emulator machine
   C64::Machine& m_emulator;

   /// processor callback; may be null
   std::shared_ptr<C64::IProcessorCallback> m_processorCallback;

   /// render window
   std::unique_ptr<RenderWindow2D> m_window;

   /// surface
   std::unique_ptr<PalettedSurface> m_surface;

   /// set to true when a new line was output
   bool m_lineUpdated;

   /// set to true when the complete screen was updated
   bool m_screenUpdated;

   /// number of raster lines rendered so far
   unsigned int m_lineCount;
};