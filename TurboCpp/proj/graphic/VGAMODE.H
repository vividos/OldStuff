// vgamode.h - VGA graphics mode
// (c) 1998, 2020 Michael Fink
// https://github.com/vividos/OldStuff/

#pragma once

#ifndef __vgamode_h_
#define __vgamode_h_

#include "graphic.h"

// VGA graphics mode 320x200 with 256 color palette
class vga_mode : graphics
{
public:
  // ctor
  vga_mode();
  // dtor
  virtual ~vga_mode();

  virtual void on();
  virtual void off();
  virtual byte far* get_framebuffer_ptr(word x, word y);
  virtual void setpix(word x, word y, const byte* pixel);
  virtual void getpix(word x, word y, byte* pixel);
  virtual void setline(word x, word y, word len, const byte* line);
  virtual void set_palette(const palette256 palette);

  // VGA specific methods

  // saves VGA mode state and video buffer
  void save_mode();

  // restores VGA mode state and video buffer
  void restore_mode();

  // gets currently set palette
  void get_palette(palette256* palette);

private:
  // frame buffer start
  byte* start;
  // saved video state
  byte* saved_state_buffer;
  // saved video buffer
  byte* saved_video_buffer;
};

// sets a 6-bit VGA palette
void set_vga_palette(const palette256 palette);

#endif
