// vgamode.cpp - VGA graphics mode
// (c) 1998,2020 Michael Fink
// https://github.com/vividos/OldStuff/

#include "vgamode.h"
#include <mem.h>

vga_mode::vga_mode()
:start((byte far*)farpointer(0xa000, 0))
{
  xres = 320;
  yres = 200;
}

vga_mode::~vga_mode()
{
}

void vga_mode::on()
{
  asm {
    mov ax, 0x13;
    int 10h
  };

  is_on = true;
}
void vga_mode::off()
{
  asm {
    mov ax, 0x3;
    int 10h
  };

  is_on = false;
}

byte far* vga_mode::get_framebuffer_ptr(word x, word y)
{
  return start + y * xres + x;
}

void vga_mode::setpix(word x, word y, const byte* pixel)
{
  start[y * xres + x] = *pixel;
}

void vga_mode::getpix(word x, word y, byte* pixel)
{
  *pixel = start[y * 320 + x];
}

void vga_mode::setline(word x, word y, word length, const byte* line)
{
  memcpy(start + y * 320 + x, line, length);
}

void vga_mode::set_palette(const palette256 palette)
{
  set_vga_palette(palette);
}

void set_vga_palette(const palette256 palette)
{
  palette256 scaled_palette;

  for (unsigned int i = 0; i < 256; i++)
    for (int j = 0; j < 3; j++)
      scaled_palette[i][j] = palette[i][j] >> 2; // VGA only uses 6 bits

  asm {
    mov ax, 0x1012
    xor bx, bx
    mov cx, 0x100
    les dx, palette
    int 10h
  };
}

void vga_mode::save_mode()
{
  word mem;
  asm {
    mov ax, 0x1c00
    mov cx, 0x7
    int 0x10
    mov mem, bx
  };

  byte far* state_buffer = (saved_state_buffer = new byte[mem << 6]);
  asm {
    mov ax, 0x1c01
    mov cx, 0x7
    les bx, state_buffer

    int 0x10
  };

  saved_video_buffer = new byte[320 * 200];
  memcpy(saved_video_buffer, start, 320 * 200);
}

void vga_mode::restore_mode()
{
  byte far* state_buffer = saved_state_buffer;
  asm {
    mov ax, 0x1c02
    mov cx, 0x7
    les bx, state_buffer
    int 0x10
  };

  memcpy(start, saved_video_buffer, 320 * 200);
  delete saved_video_buffer;
};

void vga_mode::get_palette(palette256* palette)
{
  asm {
    mov ax, 0x1017
    xor bx, bx
    mov cx, 0x100
    les dx, palette
    int 10h
  };
}
