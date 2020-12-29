// imgload.h - image loader interface
// (c) 2020 Michael Fink
// https://github.com/vividos/OldStuff/

#pragma once

#ifndef __imgload_h_
#define __imgload_h_

#include "defs.h"
#include <stdio.h>

class image;

// image loader interface
// there are two ways to load images, either by loading all data into
// memory, or (when using MS-DOS and max. allocation can be 64k), load
// each scanline into a provided memory area (possibly the linear framebuffer
// itself).
class image_loader
{
public:
  // returns an image loader instance that can load the image from given
  // filename; supported formats are: .gif, .jpg
  static image_loader* get_image_loader(const char* filename, FILE* fd);

  // dtor
  virtual ~image_loader() {}

  // when memory constrained, use:

  // reads metadata of image
  virtual bool read_metadata(word& xres, word& yres, byte& bits_per_pixel,
    bool& has_palette, byte palette[256][3]) = 0;

  // returns next Y coordinate of scanline to be read
  virtual int get_next_y() const = 0;

  // reads next scanline into given buffer
  virtual bool read_scanline(byte far* scanline, word scanline_byte_width) = 0;

  // when enough memory can be allocated at once, use:

  // loads all image data at once
  virtual image* load() = 0;
};

#endif
