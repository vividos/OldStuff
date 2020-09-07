// screen.cpp - screen class
// (c) 2020 Michael Fink
// https://github.com/vividos/OldStuff/
// This file contains reconstructed classes from lost source files that
// couldn't be recovered.

#include "screen.h"
#include <conio.h>
#include <mem.h>

// far pointer to screen RAM
static byte far* screen_ram = (byte far*)farpointer(0xb800, 0);

// size of screen RAM, for 80x25 mode
const int screen_ram_size = 80 * 25 * 2;

screen::screen()
  :stored_screen(NULL)
{
}

screen::~screen()
{
  if (stored_screen != NULL)
    restore();
}

void screen::store()
{
  if (stored_screen == NULL)
    stored_screen = new byte[screen_ram_size];
  memcpy(stored_screen, screen_ram, screen_ram_size);
}

void screen::restore()
{
  if (stored_screen == NULL)
    return;
  memcpy(screen_ram, stored_screen, screen_ram_size);

  delete stored_screen;
  stored_screen = NULL;
}

void screen::setcursortype(cursortype ct)
{
  _setcursortype(ct);
}

textwindow* screen::openwindow(point top_left, point bottom_right)
{
  ::window(top_left.x, top_left.y, bottom_right.x, bottom_right.y);
  return new textwindow();
}
