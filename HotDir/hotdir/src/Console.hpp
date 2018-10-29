//
// HotDir - Colored directory tool
// Copyright (C) 1999-2018 Michael Fink
//
/// \file Console.hpp Console output
//
#pragma once

#include <string>

/// \brief Console output
/// \details Access console and print colored text to the console. Also has a
/// built-in paging mechanism.
class Console
{
public:
   /// ctor; sets console window title
   Console(const char* title);
   /// dtor
   ~Console();

   /// Returns width of console, in characters
   unsigned int Width() const;

   /// Sets flag to wait at full pages, with custom text to output
   void SetWaitAtFullPage(bool waitAtFullPage, const char* waitText);

   /// Sets new text color
   void SetColor(unsigned char color);

   /// Prints text to console; text can contain ^ (circumflex) characters to
   /// change color; the character after the circumflex in hex specifies the
   /// new color. Also checks for full pages and waits for user to press a
   /// key.
   int Printf(const char* format, ...);

private:
   /// Prints a single character to the console
   void PrintChar(char ch);

   /// Checks if the current page is full and the wait text should be shown
   void CheckFullPage();

   /// Waits for a key press by the user
   void WaitKeypress();

private:
   /// Handle to the Win32 console screen
   HANDLE m_screen;

   /// Indicates if waiting at full page is enabled
   bool m_waitAtFullPage;

   /// Custom wait text to use
   std::string m_waitText;

   /// Number of lines already output by the console
   unsigned int m_numOutputLines;
};
