// jpegload.h - JPEG image loader
// (c) 2020 Michael Fink
// https://github.com/vividos/OldStuff/

#pragma once

#ifndef __jpegload_h_
#define __jpegload_h_

#include "imgload.h"
#include "image.h"
#include "defs.h"

// loader for JPEG image format
class jpeg_loader: public image_loader
{
public:
  // ctor
  jpeg_loader(FILE* fd);
  // dtor
  virtual ~jpeg_loader();

  // reads metadata of image
  virtual bool read_metadata(word& xres, word& yres, byte& bits_per_pixel,
    bool& has_palette, byte palette[256][3]);

  // returns next Y coordinate of scanline to be read
  virtual int get_next_y() const;

  // reads next scanline into given buffer
  virtual bool read_scanline(byte far* scanline, word scanline_byte_width);

  // loads image from file
  virtual image* load();

private:
  // JPEG decoder implementation
  struct jpeg_decoder* decoder;
};

#endif
