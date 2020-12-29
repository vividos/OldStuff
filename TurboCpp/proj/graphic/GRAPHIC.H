// graphic.h - graphics interface
// (c) 1998, 2020 Michael Fink
// https://github.com/vividos/OldStuff/

#pragma once

#ifndef __graphic_h_
#define __graphic_h_

#include "defs.h"

// palette with 256 entries
typedef byte palette256[256][3];

// graphics mode interface
class graphics
{
protected:
  word xres, yres;
  bool is_on;
public:
  // finds a suitable graphics mode, e.g. VGA or VESA mode, with given parameters
  static graphics* find_mode(word xres, word yres, byte bits_per_pixel, bool exact_match);

  // ctor
  graphics();
  // dtor
  virtual ~graphics();

  // returns x and y resolution
  word get_xres() const { return xres; }
  word get_yres() const { return yres; }

  // when a linear framebuffer is used, returns the pointer to the exact
  // pixel in the framebuffer; when not; setpix() and setline() must be used.
  virtual byte far* get_framebuffer_ptr(word x, word y) = 0;

  // switches on graphics mode
  virtual void on() = 0;

  // switches off graphics mode
  virtual void off() = 0;

  // sets single pixel color
  virtual void setpix(word x, word y, const byte* pixel) = 0;

  // returns pixel value
  virtual void getpix(word x, word y, byte* pixel) = 0;

  // sets a line of pixels
  virtual void setline(word x, word y, word length, const byte* line) = 0;

  // sets palette; depending on if the graphics mode is a paletted mode
  virtual void set_palette(const palette256 palette) = 0;
};

#endif
