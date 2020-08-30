// graphic.h - graphic support
// (c) 1997 Michael Fink
// https://github.com/vividos/OldStuff/

#ifndef __GRAPHIC_H_
#define __GRAPHIC_H_

#include "defs.h"

typedef byte palette256[256][3];
typedef byte palette16[16][3];

class graphic
{
private:
  byte far* start;
  word xres, yres;
  bool is_on;

public:
  graphic();
  ~graphic();
  word getxres() const { return xres; };
  word getyres() const { return yres; };
  void on();
  void off();
  void setpix(word x, word y, byte b);
  byte getpix(word x, word y) const;
  void setline(word x, word y, word len, const byte* b);

  void setpalette(const palette256 far* pal);
};

#endif
