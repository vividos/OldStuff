//
// c64emu - C64 Emulator using SDL
// Copyright (C) 2006-2016 Michael Fink
//
/// \file EmulatorWindow.cpp Emulator window
//

// includes
#include "StdAfx.h"
#include "EmulatorWindow.hpp"
#include "EmulatorOptions.hpp"
#include "Machine.hpp"
#include "RenderWindow2D.hpp"
#include "PalettedSurface.hpp"
#include "TapeFile.hpp"
#include "PC64File.hpp"
#include "EliteProcessorCallback.hpp"
#include "GianaSistersProcessorCallback.hpp"
#ifdef _WIN32
#include <SDL_syswm.h>
#endif
#include <algorithm>

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
   m_windowWidth(403), // PAL VIC
   m_windowHeight(284), // PAL VIC
   m_fullscreen(false),
   m_lineUpdated(false),
   m_screenUpdated(false),
   m_lineCount(0)
{
   InitSDL();

   SetFramesPerSecond(50); // try to run at 50 Hz, the speed of the PAL C64

   m_emulator.GetVideoInterfaceController().SetVideoOutputDevice(this);
}

EmulatorWindow::~EmulatorWindow()
{
   m_window.reset();

   SDL_Quit();
}

void EmulatorWindow::Configure(EmulatorOptions& options)
{
   if (options.DebugMode())
   {
      m_emulator.GetVideoInterfaceController().SetShowDebugInfo(true);
   }

   if (options.JoystickNumPadEmulation())
   {
      m_emulator.GetKeyboard().SetJoystickNumPadEmulation(true);
   }

   if (options.StartProgramCounter() != 0)
   {
      SetStartProgramCounter(options.StartProgramCounter());
   }

   SetFullscreen(options.Fullscreen());
}

void EmulatorWindow::Load(LPCTSTR filename, unsigned int entryIndex)
{
   _ftprintf(stdout, _T("Loading %s...\n"), filename);

   CString lowerFilename(filename);
   lowerFilename.MakeLower();
   lowerFilename.Trim(_T('\"'));

   if (lowerFilename.Right(4) == ".t64")
   {
      C64::TapeFile tf(lowerFilename);

      if (entryIndex == -1)
      {
         for (size_t tapeIndex = 0; tapeIndex < tf.NumTapeFiles(); tapeIndex++)
            tf.Load(tapeIndex, m_emulator.GetMemoryManager().GetRAM());
      }
      else
      {
         tf.Load(entryIndex, m_emulator.GetMemoryManager().GetRAM());
      }
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
            _ftprintf(stderr, _T("invalid .p00 file: %s\n"), filename);
         }
      }

   if (lowerFilename.Find(_T("elite")) != -1)
   {
      m_processorCallback.reset(new EliteProcessorCallback(m_emulator));
   }

   if (lowerFilename.Find(_T("giana")) != -1)
   {
      m_processorCallback.reset(new GianaSistersProcessorCallback(m_emulator));
   }
}

void EmulatorWindow::Run()
{
   if (m_emulator.GetVideoInterfaceController().GetShowDebugInfo())
   {
      m_windowWidth = 480; // show extended VIC infos
   }

   CreateEmulatorWindow();

   if (m_startProgramCounter == 0x0000)
   {
      m_startProgramCounter = FindBasicSysCommand(0x0801);

      if (m_startProgramCounter == 0)
      {
         _ftprintf(stderr, _T("couldn't find basic SYS command, searching from 0x0801\n"));
         return;
      }
   }

   m_emulator.GetProcessor().SetProgramCounter(m_startProgramCounter);

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
            // eat leading spaces
            while (memoryManager.Peek(numberAddress) == 0x20)
               numberAddress++;

            BYTE numberChar = memoryManager.Peek(numberAddress++);
            if (numberChar >= 0x30 && numberChar <= 0x39)
               address += static_cast<char>(numberChar);
            else
               break;
         }

         WORD sysAddress = static_cast<WORD>(atoi(address.c_str()));
         _ftprintf(stdout, _T("found command: %u SYS %u\n"), lineNumber, sysAddress);

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
}

void EmulatorWindow::CreateEmulatorWindow()
{
   SDL_EventState(SDL_SYSWMEVENT, SDL_ENABLE);

   int width = m_windowWidth + (m_windowWidth & 1);
   int height = m_windowHeight + (m_windowHeight & 1);

   m_window.reset(new RenderWindow2D(width, height, m_fullscreen, _T("Scale2x"), 2));
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

void EmulatorWindow::OnTick()
{
   m_screenUpdated = false;

   while (!m_screenUpdated)
   {
      m_emulator.GetVideoInterfaceController().Step();
      m_emulator.GetProcessor().Step();
   }

   m_lineUpdated = false;
}

void EmulatorWindow::OnRender()
{
   m_window->Blit(0, 0, *m_surface);
   m_window->Update();
}

void EmulatorWindow::OnEvent(SDL_Event& evt)
{
   switch (evt.type)
   {
#ifdef _WIN32
   case SDL_SYSWMEVENT:
   {
      SDL_SysWMmsg& msg = *evt.syswm.msg;
      if (msg.msg.win.msg == WM_KEYDOWN ||
         msg.msg.win.msg == WM_KEYUP)
      {
         OnKeyMessage(msg);
      }
   }
   break;
#endif

   case SDL_KEYDOWN:
      // handle key presses
      if (evt.key.keysym.sym == SDLK_x &&
         (evt.key.keysym.mod & KMOD_ALT) != 0)
      {
         MainGameLoop::QuitLoop();
      }

      if (evt.key.keysym.sym == SDLK_RETURN &&
         (evt.key.keysym.mod & KMOD_ALT) != 0)
      {
         m_fullscreen = !m_fullscreen;
         m_window->SetFullscreen(m_fullscreen);
      }
      break;

   case SDL_QUIT:
      // handle quit requests
      MainGameLoop::QuitLoop();
      break;

   default:
      break;
   }
}

void EmulatorWindow::OnKeyMessage(const SDL_SysWMmsg& msg)
{
#ifdef _WIN32
   // only handle virtual keycodes with 8 bit
   BYTE keyCode = static_cast<BYTE>(msg.msg.win.wParam & 0xFF);

   bool shiftState = ::GetKeyState(VK_SHIFT) != 0;

   if (keyCode == VK_SHIFT)
   {
      if (::GetKeyState(VK_LSHIFT) != 0)
      {
         keyCode = VK_LSHIFT;
      }
      if (::GetKeyState(VK_RSHIFT) != 0)
      {
         keyCode = VK_RSHIFT;
      }
   }

   if (keyCode == VK_CONTROL)
   {
      if (::GetKeyState(VK_LCONTROL) != 0)
      {
         keyCode = VK_LCONTROL;
      }
      if (::GetKeyState(VK_RCONTROL) != 0)
      {
         keyCode = VK_RCONTROL;
      }
   }

   m_emulator.GetKeyboard().SetKeyState(
      keyCode,
      msg.msg.win.msg == WM_KEYDOWN,
      shiftState);
#endif
}

void EmulatorWindow::OutputLine(WORD wRasterline, BYTE abLine[0x0200])
{
   ATLASSERT(wRasterline < m_windowHeight);

   Uint8* pixels = (Uint8*)m_surface->Data();
   pixels += wRasterline * m_surface->Width();

   size_t lineWidth = std::min<size_t>(0x0200, m_surface->Width());

   memcpy(pixels, abLine, lineWidth);

   m_lineUpdated = true;
   m_lineCount++;
}

void EmulatorWindow::ScreenCompleted()
{
   m_screenUpdated = true;
   m_lineCount = 0;
}
