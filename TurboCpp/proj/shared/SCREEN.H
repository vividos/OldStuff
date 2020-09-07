// screen.h - screen class
// (c) 2020 Michael Fink
// https://github.com/vividos/OldStuff/
// This file contains reconstructed classes from lost source files that
// couldn't be recovered.

#ifndef __screen_h_
#define __screen_h_

#include "cstring.h"
#include "window.h"

typedef enum cursortype
{
  nocursor = 0,
  solidcursor = 1,
  normalcursor = 2
};

// access to the text screen
class screen : public textwindow
{
public:
  screen();
  ~screen();
  void store();
  void restore();
  void setcursortype(cursortype ct);
  textwindow* openwindow(point top_left, point bottom_right);

private:
  byte* stored_screen;
};

#endif
