// window.h - textwindow class
// (c) 2020 Michael Fink
// https://github.com/vividos/OldStuff/
// This file contains reconstructed classes from lost source files that
// couldn't be recovered.

#ifndef __window_h_
#define __window_h_

#include "cstring.h"

// window point coordinates
struct point
{
  int x;
  int y;
};

typedef enum color
{
  black = 0, blue, green, cyan, red, magenta, brown, lightgray,
  darkgray, lightblue, lightgreen, lightcyan, lightred, lightmagenta,
  yellow, white
};

// text window inside a screen
class textwindow
{
public:
  // dtor
  ~textwindow();

  // clears whole window
  void clear();

  // sets cursor to position inside window
  void gotoxy(int x, int y);

  // sets text attributes for next output; background can only use colors
  // 0 to 7
  void setattr(color foreground, color background, bool blink = false);

  // outputs the given text string
  void outstr(const string& s);

  // clears rest of line
  void crlf();

private:
  friend class screen;
  textwindow();
};

#endif
