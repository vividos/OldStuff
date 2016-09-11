//
// c64emu - C64 Emulator using SDL
// Copyright (C) 2006-2016 Michael Fink
//
/// \file EmulatorWindow.cpp Emulator window
//

// includes
#include "StdAfx.h"
#include "EmulatorWindow.hpp"
#include "Machine.hpp"
#include "RenderWindow2D.hpp"
#include "PalettedSurface.hpp"
#include "TapeFile.hpp"
#include "PC64File.hpp"
#include "EliteProcessorCallback.hpp"

/// C64 color map
std::array<Uint8, 16 * 3> colormap =
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

EmulatorWindow::EmulatorWindow(C64::Machine& emulator)
   :MainGameLoop(true, _T("C64 Emulator")),
   m_emulator(emulator),
   m_lineUpdated(false),
   m_screenUpdated(false),
   m_lineCount(0)
{
   _ftprintf(stdout, _T("C64 Emulator\n"));

   InitSDL();

   m_emulator.GetVideoInterfaceController().SetVideoOutputDevice(this);
}

EmulatorWindow::~EmulatorWindow()
{
   m_window.reset();

   SDL_Quit();
}

void EmulatorWindow::Load(LPCTSTR filename)
{
   _ftprintf(stdout, _T("Loading %s..."), filename);

   CString lowerFilename(filename);
   lowerFilename.MakeLower();
   lowerFilename.Trim(_T('\"'));

   if (lowerFilename.Right(4) == ".t64")
   {
      C64::TapeFile tf(lowerFilename);

      tf.Load(0, m_emulator.GetMemoryManager().GetRAM());
   }
   else
      if (lowerFilename.Right(4) == ".p00" ||
         lowerFilename.Right(4) == ".prg")
      {
         C64::PC64File pf(lowerFilename);

         if (pf.IsValid())
         {
            pf.Load(m_emulator.GetMemoryManager().GetRAM());
         }
         else
         {
            _ftprintf(stderr, _T("invalid .p00 file: %s"), filename);
         }
      }

   if (lowerFilename.Find(_T("elite")) != -1)
   {
      m_processorCallback.reset(new EliteProcessorCallback(m_emulator));
   }
}

void EmulatorWindow::Run()
{
   WORD startProgramCounter =
      FindBasicSysCommand(0x0801);

   if (startProgramCounter == 0)
   {
      _ftprintf(stderr, _T("couldn't find basic SYS command, searching from 0x0801"));
      return;
   }

   m_emulator.GetProcessor().SetProgramCounter(startProgramCounter);

   if (m_processorCallback != nullptr)
      m_emulator.GetProcessor().AddProcessorCallback(m_processorCallback.get());

   MainGameLoop::Run();

   if (m_processorCallback != nullptr)
      m_emulator.GetProcessor().RemoveProcessorCallback(m_processorCallback.get());
}

WORD EmulatorWindow::FindBasicSysCommand(WORD startAddress)
{
   const C64::MemoryManager& memoryManager = m_emulator.GetMemoryManager();

   WORD currentAddress = startAddress;

   do
   {
      WORD lineNumber = memoryManager.Peek16(currentAddress + 2);
      BYTE basicCommand = memoryManager.Peek(currentAddress + 4);
      if (basicCommand == 0x9E) // SYS
      {
         std::string address;
         WORD numberAddress = currentAddress + 5;
         while (numberAddress < currentAddress + 5 + 5)
         {
            BYTE numberChar = memoryManager.Peek(numberAddress++);
            if (numberChar >= 0x30 && numberChar <= 0x39)
               address += static_cast<char>(numberChar);
            else
               break;
         }

         WORD sysAddress = static_cast<WORD>(atoi(address.c_str()));
         _ftprintf(stdout, _T("found command: %u SYS %u"), lineNumber, sysAddress);

         return sysAddress;
      }

      currentAddress = memoryManager.Peek16(currentAddress);

   } while (currentAddress != 0);

   return 0x0000;
}

void EmulatorWindow::InitSDL()
{
   int ret = SDL_Init(SDL_INIT_VIDEO);
   if (ret < 0)
   {
      ATLTRACE(_T("couldn't init SDL\n"));
      return;
   }

   int width = 0x0200, height = 312;

   m_window.reset(new RenderWindow2D(width, height, false, _T("Scale2x"), 2));
   if (m_window == nullptr)
   {
      ATLTRACE(_T("couldn't set video mode\n"));
      return;
   }

   m_surface.reset(new PalettedSurface(width, height, m_window->BitsPerPixel()));

   m_surface->SetPalette<16>(colormap);
}

void EmulatorWindow::UpdateCaption(const CString& caption)
{
   m_window->SetCaption(caption);
}

void EmulatorWindow::OnRender()
{
   m_screenUpdated = false;

#ifdef _DEBUG
   unsigned int t1 = SDL_GetTicks();
#endif

   while (!m_screenUpdated)
   {
      m_emulator.GetVideoInterfaceController().Step();
      m_emulator.GetProcessor().Step();
   }

#ifdef _DEBUG
   unsigned int t2 = SDL_GetTicks();
   ATLTRACE(_T("line %u took %u ms to render\n"), m_lineCount, t2 - t1);
#endif

   m_lineUpdated = false;

   m_window->Blit(0, 0, *m_surface);
   m_window->Update();
}

void EmulatorWindow::OnEvent(SDL_Event& evt)
{
   switch (evt.type)
   {
   case SDL_KEYDOWN:
      // handle key presses
      if (evt.key.keysym.sym == SDLK_x &&
         (evt.key.keysym.mod & KMOD_ALT) != 0)
         MainGameLoop::QuitLoop();
      break;

   case SDL_QUIT:
      // handle quit requests
      MainGameLoop::QuitLoop();
      break;

   default:
      break;
   }
}

void EmulatorWindow::OutputLine(WORD wRasterline, BYTE abLine[0x0200])
{
   Uint8* pixels = (Uint8*)m_surface->Data();
   pixels += wRasterline * m_surface->Width();

   memcpy(pixels, abLine, 0x0200);

//   m_window->Blit(0, 0, *m_surface);

   m_lineUpdated = true;
   m_lineCount++;
}

void EmulatorWindow::ScreenCompleted()
{
   m_screenUpdated = true;
   m_lineCount = 0;
}