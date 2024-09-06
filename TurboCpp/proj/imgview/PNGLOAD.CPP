// pngload.cpp - png image loader
// (c) 2021 Michael Fink
// https://github.com/vividos/OldStuff/

#include "pngload.h"
#include "altpng.h"
#include <mem.h>

png_loader::png_loader(FILE* _fd)
  :fd(_fd),
  current_scanline_y(0)
{
  altpng_stream_init(&stream, 1, _fd);
  altpng_decoder_init(&decoder, &stream, this);
}

png_loader::~png_loader()
{
  altpng_decoder_finish(&decoder);
  altpng_stream_finish(&stream);
  fclose(fd);
}

bool png_loader::read_metadata(word& xres, word& yres, byte& bits_per_pixel,
  bool& has_palette, byte palette[256][3])
{
  int ret = altpng_decoder_header(&decoder);
  if (ret != ape_ok)
    return false;

  xres = decoder.image_info.width;
  yres = decoder.image_info.height;
  bits_per_pixel = decoder.image_info.bit_depth;
  has_palette = decoder.image_info.palette != NULL;

  if (has_palette)
  {
    memcpy(palette, decoder.image_info.palette, 256 * 3);
  }

  return true;
}

int png_loader::get_next_y() const
{
  return current_scanline_y < decoder.image_info.height
    ? current_scanline_y
    : -1;
}

bool png_loader::read_scanline(byte far* scanline, word scanline_byte_width)
{
  current_scanline = scanline;
  current_scanline_size = scanline_byte_width;

  decoder.process_scanline = pngload_scanline_store_single;

  int ret = altpng_decoder_decode(&decoder);

  current_scanline_y++;

  return ret == ape_ok || ape_scanline_yield;
}

int png_loader::pngload_scanline_store_single(
  altpng_decoder_ptr decoder, altpng_scanline* scanline)
{
  png_loader& loader = *(png_loader*)decoder->user_data;

  loader.store_scanline(scanline->scanline, scanline->sl_size);

  return ape_scanline_yield;
}

// stores all scanlines in the allocated image
int png_loader::pngload_scanline_store_all(
  altpng_decoder_ptr decoder, altpng_scanline* scanline)
{
  png_loader& loader = *(png_loader*)decoder->user_data;

  loader.store_scanline(scanline->scanline, scanline->sl_size);

  return ape_ok;
}

void png_loader::store_scanline(const char* scanline, size_t length)
{
  // convert to 8 bit bgr and store
  // TODO
}

image* png_loader::load()
{
  word xres;
  word yres;
  byte bits_per_pixel;
  bool has_palette;
  byte palette[256][3];

  if (!read_metadata(xres, yres, bits_per_pixel,
    has_palette, palette))
    return NULL;

  image* img = new image(xres, yres,
    bits_per_pixel, true);

  if (has_palette)
  {
    memcpy(img->get_palette(), palette, 256 * 3);
  };

  decoder.process_scanline = pngload_scanline_store_all;

  int ret = altpng_decoder_decode(&decoder);

  if (ret != ape_ok)
  {
    delete img;
    img = NULL;
  }

  return img;
}
