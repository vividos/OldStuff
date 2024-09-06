// pngload.h - png image loader
// (c) 2021 Michael Fink
// https://github.com/vividos/OldStuff/

#pragma once

#ifndef __pngload_h_
#define __pngload_h_

#include "imgload.h"
#include "image.h"
#include "defs.h"
#include <stdio.h>
#include "altpng.h"

// loader for png image format
class png_loader : public image_loader
{
public:
  // ctor
  png_loader(FILE* fd);
  // dtor
  virtual ~png_loader();

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
  // stores a single scanline and yields decoding
  static int pngload_scanline_store_single(
    altpng_decoder_ptr decoder, altpng_scanline* scanline);

  // stores a single scanline and returns to decoding the next line
  static int pngload_scanline_store_all(
    altpng_decoder_ptr decoder, altpng_scanline* scanline);

  void store_scanline(const char* scanline, size_t length);

private:
  // file to read from
  FILE* fd;

  // decoder instance
  altpng_decoder decoder;

  // stream instance
  altpng_stream stream;

  byte far* current_scanline;
  size_t current_scanline_size;

  // when reading by scanline, this contains next number of next scanline
  word current_scanline_y;
};

#endif
