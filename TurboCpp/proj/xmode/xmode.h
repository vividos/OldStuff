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

  byte activepage();
  void showpage(byte page);
  void setpage(byte page);
  void switchpage();
  void clearpage(byte page);
  void pagemove(byte page);
};

#endif
