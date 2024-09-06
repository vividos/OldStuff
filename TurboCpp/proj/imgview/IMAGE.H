// image.h - image class
// (c) 2020,2021 Michael Fink
// https://github.com/vividos/OldStuff/

#pragma once

#ifndef __image_h_
#define __image_h_

#include "defs.h"

class image_loader;

// image, with palete or true-color image
class image
{
public:
  // loads an image from given filename; supported formats are: .gif, .jpg
  static image* load(const char* filename);

  word get_xres() const { return xres; }
  word get_yres() const { return yres; }

  // returns raw image data
  byte* get_data() { return data; }

  // returns palette with 256 entries, or NULL when true-color image
  byte* get_palette() { return palette; }

private:
  friend class gif_loader;
  friend class jpeg_loader;
  friend class png_loader;

  // creates a new image object, either with palette, or as true-color image
  image(unsigned int width, unsigned int height,
    unsigned char bits_per_pixel, bool has_palette);

  // x and y resolution
  word xres;
  word yres;

  // image data
  byte* data;

  // palette data
  byte* palette;
};

#endif
