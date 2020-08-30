// xmode.h - Mode X support
// (c) 1999 Michael Fink
// https://github.com/vividos/OldStuff/
// The xmode class is a wrapper for the 320x240 VGA Mode X. Up to four
// pages of graphics are supported.

#ifndef __xmode_h_
#define __xmode_h_

#include "defs.h"

class xmode
{
  bool mode_on;
public:
  xmode();
  ~xmode();

  void on();
  void off();

  void setpix(word x, word y, byte farbe);
  byte getpix(word x, word y);

  void setline(word x, word y, byte* buffer, word length);

  // returns currently active page (0-3)
  byte activepage();

  // shows new page to the user
  void showpage(byte page);

  // sets page for get and set functions
  void setpage(byte page);

  // switches between pages 0 and 1
  void switchpage();

  // clears given page with black color
  void clearpage(byte page);

  // copies given page to currently set page
  void pagemove(byte page);

  // prints a single character to the specified coordinates
  void printchar(char ch, word x, word y, byte foreground, byte background);

  // prints a text string to the specified coordinates
  void print(word x, word y, byte foreground, byte background, const char* text);
};

#endif
