// vesamode.h - VESA graphics mode
// (c) 1997, 2020 Michael Fink
// https://github.com/vividos/OldStuff/

#pragma once

#ifndef __vesa_mode_h_
#define __vesa_mode_h_

#include "defs.h"
#include "graphic.h"

struct vesa_vbe_info_block;
struct vesa_mode_info_block;

// base class for VESA graphics modes
// http://www.phatcode.net/res/221/files/vbe20.pdf
class vesa_mode : public graphics
{
public:
  // returns if VESA bios extensions are available at all
  static bool is_avail();

  // prints out all available modes using printf
  static void show_modes();

  // finds a suitable graphics mode, with given parameters
  static vesa_mode* find_mode(word xres, word yres,
    byte bits_per_pixel, bool exact_match);

  // ctor common for all vesa modes
  vesa_mode(vesa_mode_info_block* mib, word mode);

  // dtor
  virtual ~vesa_mode();

  // override some virtual methods that are common to all vesa modes

  virtual void on();
  virtual void off();
  virtual byte far* get_framebuffer_ptr(word x, word y);
  virtual void getpix(word x, word y, byte* pixel);
  virtual void set_palette(const palette256 palette);
  virtual void setpix(word x, word y, const byte* pixel);
  virtual void setline(word x, word y, word
    length_in_bytes, const byte* line);

  // saves state of the video mode, e.g. to change to text mode
  void save_mode();
  // restores video mode state
  void restore_mode();

private:
  // retrieves vesa vbe info block
  static bool get_vbe_info_block(vesa_vbe_info_block far* vib);

  // returns currently used vesa mode, if any
  static word get_current_vesa_mode();

  // retrieves vesa mode info block for given mode
  static bool get_mode_info_block(
    vesa_mode_info_block far* mib, word mode);

  // creates a graphics mode object from given vesa mode
  static vesa_mode* create_mode(
    vesa_mode_info_block far* mib, word mode);

protected:
  // calculates buffer position in window or linear framebuffer
  byte far* get_buffer_pos(word x, word y);

  // sets new page
  void set_page(word page);

protected:
  // segment of frame buffer
  word segment;

private:
  // indicates if video mode has a linear framebuffer
  bool has_linear_framebuffer;

  // start of framebuffer, either the window, or the linear framebuffer
  byte far* start;

  // last page that was set
  word last_page;

  // mode word
  word mode;

  // bytes pre scanline
  word bytes_per_line;

  // window granularity, in kb
  word granularity_kb;

  // number of bits per pixel
  byte bits_per_pixel;

  // function to set frame in windowed mode
  void (far* setframe)();

  // saved video state; used in save_mode()/restore_mode()
  byte far* saved_state_buffer;
};

// vesa mode implementation for 8-bit pixel formats
// these modes use a palette
class vesa_mode_8bit : public vesa_mode
{
public:
  vesa_mode_8bit(vesa_mode_info_block* mib, word mode)
    :vesa_mode(mib, mode)
  {
  }

  virtual void set_palette(const palette256 palette);
};

// vesa mode implementation for 24-bit pixel formats
// these are true-color modes not using a palette
// pixels are stored as RR GG BB values
class vesa_mode_24bit : public vesa_mode
{
public:
  vesa_mode_24bit(vesa_mode_info_block* mib, word mode)
    : vesa_mode(mib, mode)
  {
  }
};

// vesa mode implementation for 15 and 16-bit pixel formats
// the pixel and line pointers point to word sized values, and color values
// are stored using 555 (15-bit) or 565 (16-bit) bit patterns
class vesa_mode_16bit : public vesa_mode
{
public:
  vesa_mode_16bit(vesa_mode_info_block* mib, word mode)
    : vesa_mode(mib, mode)
  {
  }
};

// vesa mode implementation for 4-bit pixel formats
// the pixel and line parameters still contain pointer to bytes
class vesa_mode_4bit : public vesa_mode
{
public:
  vesa_mode_4bit(vesa_mode_info_block* mib, word mode)
    : vesa_mode(mib, mode)
  {
  }

  virtual void setpix(word x, word y, const byte* pixel);
  virtual void setline(word x, word y, word length, const byte* line);
  virtual void set_palette(const palette256 palette);
};

#endif
