// gifload.cpp - GIF image loader
// (c) 1996, 2020 Michael Fink
// https://github.com/vividos/OldStuff/

#include "gifload.h"
#include "defs.h"
#include "cstring.h"
#include "lzwdecod.h"
#include <stdio.h>
#include <mem.h>
#include <assert.h>

// identifier starting a extension block, also known as extension introducer.
// all extension blocks begin with this value
const byte graphics_control_extension_indentifier = 0x21;

// label that starts a graphics control extension block
const byte graphic_control_label = 0xf9;

// label that starts a plain text extension block
const byte plain_text_label = 0x01;

// label that starts an application extension block
const byte application_extension_label = 0xff;

// label that starts a comment extension block
const byte comment_extension_label = 0xfe;

// image separator; begins a new image and an image descriptor follows
const byte image_separator = 0x2c;

// trailer block marker
const byte trailer_marker = 0x3b;

gif_loader::gif_loader(FILE* _fd)
  :fd(_fd),
  current_scanline_y(0),
  decoder(NULL)
{
}

gif_loader::~gif_loader()
{
  delete decoder;
}

bool gif_loader::read_metadata(word& xres, word& yres, byte& bits_per_pixel,
  bool& has_palette, byte palette[256][3])
{
  read_header();

  xres = header.lsd.xres;
  yres = header.lsd.yres;
  bits_per_pixel = header.lsd.flags.color_resolution + 1;
  has_palette = header.lsd.flags.global_color_table;

  if (header.lsd.flags.global_color_table)
  {
    memcpy(palette, global_palette, 256 * 3);
  }

  bool has_local_palette = false;
  float animation_delay = 0.f;
  if (!read_next_image_metadata(
    has_local_palette, local_palette, animation_delay))
    return false;

  if (!has_palette && !has_local_palette)
    return false; // no global + local palette

  if (has_local_palette)
  {
    memcpy(palette, local_palette, 256 * 3);
    has_palette = true;
  }

  byte compression_size = fgetc(fd);
  delete decoder;
  decoder = new lzw_decoder(fd, compression_size);

  return true;
}

int gif_loader::get_next_y() const
{
  return current_scanline_y < idb.yres
    ? current_scanline_y
    : -1;
}

bool gif_loader::read_scanline(byte far* scanline, word scanline_byte_width)
{
  assert(decoder != NULL); // can't mix scanline reading with image reading

  if (scanline_byte_width < idb.xres)
    return false;

  decoder->get_block(&scanline[idb.xorg], idb.xres);

  current_scanline_y++;

  return true;
}

image* gif_loader::load()
{
  if (!read_header())
    return NULL;

  img = new image(header.lsd.xres, header.lsd.yres,
    header.lsd.flags.color_resolution + 1, true);

  if (header.lsd.flags.global_color_table)
  {
    memcpy(img->get_palette(), global_palette, 256 * 3);
  };

  float animation_delay = 0.f;
  image* local_img = read_next_image(animation_delay);

  return local_img;
}

bool gif_loader::read_header()
{
  fread(&header, 1, sizeof(gif_header), fd);

  if (0 != memcmp(header.signature, "GIF87a", 6) &&
    0 != memcmp(header.signature, "GIF89a", 6))
    return false;

  if (header.lsd.flags.global_color_table)
  {
    read_palette(header.lsd.flags.size_color_table, global_palette);
  };

  return true;
}

void gif_loader::read_palette(byte size_color_table, byte palette[256][3])
{
  word table_size = 1 << (size_color_table + 1);
  fread(palette, 3, table_size, fd);
}

bool gif_loader::read_next_image_metadata(bool& has_palette,
  byte palette[256][3], float& animation_delay)
{
  byte identifier = fgetc(fd);

  while (identifier != trailer_marker)
  {
    if (identifier == graphics_control_extension_indentifier)
    {
      skip_extension_block();
    }
    else if (identifier == image_separator)
    {
      current_scanline_y = 0;
      read_image_descriptor_metadata(has_palette, palette);
      return true;
    }
    else if (identifier == trailer_marker)
    {
      break;
    }

    identifier = fgetc(fd);
  }

  return false;
}

image* gif_loader::read_next_image(float& animation_delay)
{
  bool has_local_palette = false;
  if (!read_next_image_metadata(has_local_palette, local_palette, animation_delay))
    return NULL;

  if (has_local_palette)
    memcpy(img->get_palette(), local_palette, 256 * 3);

  image* local_img = read_image_data();
  return local_img;
}

void gif_loader::skip_extension_block()
{
  fgetc(fd); // ignore the label

  // skip over sub data blocks
  byte length = fgetc(fd);
  while (length > 0)
  {
    fseek(fd, length, SEEK_CUR);
    length = fgetc(fd);
  }
}

void gif_loader::read_image_descriptor_metadata(
  bool& has_palette, byte palette[256][3])
{
  fread(&idb, 1, sizeof(image_descriptor), fd);

  has_palette = idb.flags.local_color_table;
  if (idb.flags.local_color_table)
  {
    byte local_palette[256][3] = { 0 };
    read_palette(idb.flags.size_color_table, local_palette);
    memcpy(palette, local_palette, 256 * 3);
  }
  else
    memcpy(palette, global_palette, 256 * 3);
}

image* gif_loader::read_image_data()
{
  byte compression_size = fgetc(fd);

  lzw_decoder decoder(fd, compression_size);

  for (word y = 0; y < idb.yres; y++)
  {
    byte* pixbuf =
      img->get_data() + (size_t)header.lsd.xres * y;

    decoder.get_block(&pixbuf[idb.xorg], idb.xres);
  }

  return img;
}
