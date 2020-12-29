// graphic.cpp - graphics interface
// (c) 1998, 2020 Michael Fink
// https://github.com/vividos/OldStuff/

#include "graphic.h"
#include "vgamode.h"
#include "vesamode.h"

graphics::graphics()
:is_on(false)
{
}

graphics::~graphics()
{
  if (is_on)
    off();
}

graphics* graphics::find_mode(word xres, word yres,
  byte bits_per_pixel, bool exact_match)
{
  if (exact_match && xres == 320 && yres == 200 && bits_per_pixel == 8)
    return new vga_mode();

  if (!exact_match && xres <= 320 && yres <= 200 && bits_per_pixel == 8)
    return new vga_mode();

  if (vesa_mode::is_avail())
  {
    vesa_mode* mode = vesa_mode::find_mode(xres, yres, bits_per_pixel, exact_match);
    if (mode != NULL)
      return mode;
  }

  return NULL;
}
