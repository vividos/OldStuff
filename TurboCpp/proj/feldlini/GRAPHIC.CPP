// graphic.cpp - graphic support
// (c) 1997 Michael Fink

#include "graphic.h"
#include <mem.h>
#include <assert.h>

graphic::graphic()
{
  start = (byte far*)(0xa000 << 16);
  xres = 320;
  yres = 200;
  is_on = false;
}

graphic::~graphic()
{
  if (is_on)
    off();
}

void graphic::on()
{
  asm {
    mov ax, 0x13;
    int 10h
  };
  is_on = true;
}

void graphic::off()
{
  asm {
    mov ax, 0x3;
    int 10h
  };

  is_on = false;
}

void graphic::setpix(word x, word y, byte b)
{
  assert(x < xres);
  assert(y < yres);

  start[y * xres + x] = b;
}

byte graphic::getpix(word x, word y) const
{
  assert(x < xres);
  assert(y < yres);

  return start[y * 320 + x];
}

void graphic::setline(word x, word y, word len, const byte* b)
{
  assert(x + len < xres);
  assert(y < yres);

  memcpy(start + y * xres + x, b, len);
}

void graphic::setpalette(const palette256 far* pal)
{
  asm {
    mov ax, 0x1012;
    xor bx, bx;
    mov cx, 0x100;
    les dx, pal;
    int 10h
  };
};
