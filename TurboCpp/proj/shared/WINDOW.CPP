// window.cpp - textwindow class
// (c) 2020 Michael Fink
// https://github.com/vividos/OldStuff/
// This file contains reconstructed classes from lost source files that
// couldn't be recovered.

#include "window.h"
#include <conio.h>

textwindow::textwindow()
{
}

textwindow::~textwindow()
{
  text_info ti;
  gettextinfo(&ti);
  if (ti.winleft != 1 || ti.wintop != 1 ||
      ti.winright != ti.screenwidth ||
      ti.winbottom != ti.screenheight)
  {
    ::window(1, 1, ti.screenwidth, ti.screenheight);
  }
}

void textwindow::clear()
{
  ::clrscr();
}

void textwindow::gotoxy(int x, int y)
{
  ::gotoxy(x, y);
}

void textwindow::setattr(color foreground, color background,
  bool blink)
{
  textattr((foreground & 15) |
    ((background & 7) << 4) |
    (blink ? BLINK : 0));
}

void textwindow::outstr(const string& s)
{
  cprintf(s.getstr());
}

void textwindow::crlf()
{
  clreol();
}
