// gifload.h - GIF image loader
// (c) 1996, 2020 Michael Fink
// https://github.com/vividos/OldStuff/
// See: https://www.w3.org/Graphics/GIF/spec-gif89a.txt
// Also: http://matthewflickinger.com/lab/whatsinagif/index.html

#pragma once

#ifndef __gifload_h_
#define __gifload_h_

#include "imgload.h"
#include "image.h"
#include "defs.h"
#include <stdio.h>

class lzw_decoder;

#pragma option -a-
#pragma option -b-

// GIF header
struct gif_header
{
  // signature and version bytes; either GIF87a or GIF89a
  char signature[6];

  struct logical_screen_descriptor
  {
    // logical screen width and height
    word xres, yres;

    // logical screen descriptor flags
    struct lsd_flags
    {
      byte size_color_table : 3; // size of color table: 2^(N+1) entries
      byte sorted_color_table : 1; // bit 3: is global palette pre-sorted?
      byte color_resolution : 3; // color resolution in bits, reduced by 1
      bool global_color_table : 1; // bit 7: global palette available?
    } flags;

    // background color index in palette
    byte background_color_index;

    // pixel aspect ratio
    byte pixel_aspect_ratio;
  } lsd;
};

// image descriptor, specifying a (sub) image
// this struct is prefixed by 0x2C
struct image_descriptor
{
  // origin coordinates inside the logical screen
  word xorg, yorg;

  // resolution of the image; must fit into logical screen size
  word xres, yres;

  // image descriptor flags
  struct image_descriptor_flags
  {
    byte size_color_table : 3; // size of color table: 2^(N+1) entries
    byte reserved : 2; // reserved bits
    bool sorted_color_table : 1; // bit 5: is local palette pre-sorted?
    bool interlaced : 1; // bit 6: indicates if image is stored interlaced
    bool local_color_table : 1; // bit 7: indicates if a local color table
    // is present
  } flags;
};

#pragma option -a
#pragma option -b


// loader for GIF image format
class gif_loader : public image_loader
{
public:
  // ctor
  gif_loader(FILE* fd);
  // dtor
  virtual ~gif_loader();

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
  // reads GIF header and global palette, if specified
  bool read_header();

  // reads a palette with given size
  void read_palette(byte size_color_table, byte palette[256][3]);

  // reads blocks until the next image and returns image metadata
  bool read_next_image_metadata(bool& has_palette,
    byte palette[256][3], float& animation_delay);

  // reads next image at current file pointer
  image* read_next_image(float& animation_delay);

  // skips over the following extension block
  void skip_extension_block();

  // reads metadata from image descriptor
  void read_image_descriptor_metadata(
    bool& has_palette, byte palette[256][3]);

  // reads image descriptor and decodes image
  image* read_image_data();

private:
  // file to read from
  FILE* fd;

  // header of GIF file
  gif_header header;

  // global palette from header
  byte global_palette[256][3];

  // current image descriptor
  image_descriptor idb;

  // local palette of current image
  byte local_palette[256][3];

  // current image
  image* img;

  // when reading by scanline, this contains next number of next scanline
  word current_scanline_y;

  // current LZW decoder
  lzw_decoder* decoder;
};

#endif
