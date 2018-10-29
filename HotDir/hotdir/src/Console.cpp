//
// HotDir - Colored directory tool
// Copyright (C) 1999-2018 Michael Fink
//
/// \file Console.cpp Console output
//
#include "pch.h"
#include "Console.hpp"
#include <cstdarg>
#include <vector>
#include <wincon.h>

Console::Console(const char* title)
   :m_screen(GetStdHandle(STD_OUTPUT_HANDLE)),
   m_waitAtFullPage(false),
   m_numOutputLines(0)
{
   ::SetConsoleTitleA(title);
}

Console::~Console()
{
   CloseHandle(m_screen);
}

unsigned int Console::Width() const
{
   CONSOLE_SCREEN_BUFFER_INFO csbi = { 0 };
   GetConsoleScreenBufferInfo(m_screen, &csbi);

   unsigned int windowWidth = static_cast<unsigned int>(csbi.srWindow.Right - csbi.srWindow.Left);

   return windowWidth;
}

void Console::SetWaitAtFullPage(bool waitAtFullPage, const char* waitText)
{
   m_waitAtFullPage = waitAtFullPage;
   m_waitText = waitText;
}

void Console::SetColor(unsigned char color)
{
   ::SetConsoleTextAttribute(m_screen, color);
}

int Console::Printf(const char* format, ...)
{
   va_list argptr;

   va_start(argptr, format);
   int count = _vscprintf(format, argptr);
   va_end(argptr);

   std::vector<char> buffer(static_cast<size_t>(count) + 1);

   va_start(argptr, format);
   count = vsprintf_s(buffer.data(), buffer.size(), format, argptr);
   va_end(argptr);

   for (int i = 0; buffer[i]; i++)
   {
      if (buffer[i] == '^') {
         unsigned char color = buffer[++i] & 0x5f;
         if (color == 0)
            break;

         color -= color > 0x20 ? 55 : 0x10;
         SetColor(color);
      }
      else
      {
         PrintChar(buffer[i]);
      }
   }

   return count;
}

void Console::PrintChar(char ch)
{
   DWORD dummy = 0;
   LPVOID dummy2 = nullptr;
   WriteConsoleA(m_screen, (CONST VOID*)&ch, 1, &dummy, dummy2);

   // check if we're at the last line
   if (m_waitAtFullPage)
      CheckFullPage();
}

void Console::CheckFullPage()
{
   CONSOLE_SCREEN_BUFFER_INFO csbi = { 0 };
   GetConsoleScreenBufferInfo(m_screen, &csbi);

   if (csbi.dwCursorPosition.X == 0)
      m_numOutputLines++; // cr occured

   unsigned int windowHeight = static_cast<unsigned int>(csbi.srWindow.Bottom - csbi.srWindow.Top);

   if (csbi.dwCursorPosition.X == 0 &&
      csbi.dwCursorPosition.Y == csbi.srWindow.Bottom &&
      m_numOutputLines >= windowHeight)
   {
      ::SetConsoleTextAttribute(m_screen, 0x07);

      DWORD dummy = 0;
      LPVOID dummy2 = nullptr;
      WriteConsoleA(
         m_screen,
         (CONST VOID*)m_waitText.data(),
         static_cast<DWORD>(m_waitText.size()),
         &dummy,
         dummy2);

      WaitKeypress();

      m_numOutputLines = 0;
      PrintChar('\n');
      m_numOutputLines = 0;
   }
}

void Console::WaitKeypress()
{
   HANDLE screenInput = GetStdHandle(STD_INPUT_HANDLE);

   FlushConsoleInputBuffer(screenInput);

   DWORD previousConsoleMode = 0;
   GetConsoleMode(screenInput, &previousConsoleMode);

   SetConsoleMode(screenInput, previousConsoleMode & ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT));

   char b[2];
   b[0] = 0;

   do
   {
      ULONG dummy1 = 0;
      ReadConsole(screenInput, (LPVOID)b, 1, &dummy1, nullptr);
   } while (b[0] == 0 && b[1] == 0);

   SetConsoleMode(screenInput, previousConsoleMode);
}
