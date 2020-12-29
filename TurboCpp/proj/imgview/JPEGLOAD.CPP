// jpegload.cpp - JPEG image loader
// (c) 2020 Michael Fink
// https://github.com/vividos/OldStuff/

#include "jpegload.h"
#pragma option -a-
#pragma option -b-
#include <jpeglib.h>
#pragma option -a
#pragma option -b

#include <assert.h>
#include <mem.h>
#include "image.h"

inline void swap_bytes(byte& b1, byte& b2)
{
  register byte temp = b1;
  b1 = b2;
  b2 = temp;
}

// JPEG decoder that uses libjpeg
struct jpeg_decoder
{
  // ctor
  jpeg_decoder(FILE* fd)
  {
    memset(&cinfo, 0, sizeof(error_mgr));

    cinfo.err = jpeg_std_error(&error_mgr);
    cinfo.err->error_exit = &on_error;
    cinfo.err->output_message = &output_message;

    jpeg_create_decompress(&cinfo);

    jpeg_stdio_src(&cinfo, fd);
  }

  // dtor
  ~jpeg_decoder()
  {
    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
  }

  // initializes decoder
  bool init();

  // reads in one scanline
  bool read_scanline(JSAMPLE* scanline, size_t scanline_byte_length);

  // reads in all scanline and stores them in the image
  void read_scanlines(image& img);

  // called to show an error while decoding
  static void on_error(j_common_ptr cinfo)
  {
    (*cinfo->err->output_message) (cinfo);
    jpeg_destroy(cinfo);
  }

  // outputs a message
  static void output_message(j_common_ptr cinfo)
  {
    char buffer[JMSG_LENGTH_MAX];
    cinfo->err->format_message(cinfo, buffer);

    printf(buffer);
  }

  jpeg_error_mgr error_mgr;
  jpeg_decompress_struct cinfo;
};

bool jpeg_decoder::init()
{
  int ret = jpeg_read_header(&cinfo, TRUE);
  if (ret != JPEG_HEADER_OK)
    return false;

  // set parameters for decompression
  if (cinfo.num_components != 3)
    cinfo.out_color_space = JCS_RGB; // convert grayscale to RGB

  boolean b = jpeg_start_decompress(&cinfo);
  if (b != TRUE)
    return false;

  assert(cinfo.output_components == 3);

  return true;
}

bool jpeg_decoder::read_scanline(JSAMPLE* scanline, size_t scanline_byte_length)
{
  JDIMENSION dim = jpeg_read_scanlines(&cinfo, &scanline, 1);
  if (dim != 1)
    return false;

  // convert from BGR to RGB
  for (size_t index = 0; index < scanline_byte_length; index += 3)
    swap_bytes(scanline[index], scanline[index + 2]);

  return true;
}

void jpeg_decoder::read_scanlines(image& img)
{
  size_t row_stride = cinfo.output_width * cinfo.output_components;

  JSAMPLE* scanline = new JSAMPLE[row_stride];

  word y = 0;
  while (cinfo.output_scanline < cinfo.output_height)
  {
    if (!read_scanline(scanline, row_stride))
      break;

    // TODO or directly use image pointer?
    byte* pixel_buffer =
      img.get_data() + (size_t)row_stride * y;

    memcpy(pixel_buffer, scanline, row_stride);

    y++;
  }

  delete scanline;
}

jpeg_loader::jpeg_loader(FILE* fd)
  :decoder(new jpeg_decoder(fd))
{
}

jpeg_loader::~jpeg_loader()
{
  delete decoder;
}

#pragma argsused
bool jpeg_loader::read_metadata(word& xres, word& yres, byte& bits_per_pixel,
  bool& has_palette, byte palette[256][3])
{
  if (!decoder->init())
    return false;

  // true-color, 24-bit, no palette
  xres = decoder->cinfo.output_width;
  yres = decoder->cinfo.output_height;
  bits_per_pixel = 24;
  has_palette = false;

  return true;
}

int jpeg_loader::get_next_y() const
{
  return decoder->cinfo.output_scanline < decoder->cinfo.output_height
    ? decoder->cinfo.output_scanline
    : -1;
}

bool jpeg_loader::read_scanline(byte far* raw_scanline, word scanline_byte_width)
{
  size_t row_stride = decoder->cinfo.output_width * decoder->cinfo.output_components;
  if (row_stride != scanline_byte_width)
    return false;

  JSAMPLE* scanline = (JSAMPLE*)raw_scanline;

  return decoder->read_scanline(scanline, row_stride);
}

image* jpeg_loader::load()
{
  if (!decoder->init())
    return NULL;

  // true-color, 24-bit, no palette
  image* img = new image(
    decoder->cinfo.output_width,
    decoder->cinfo.output_height,
    24, false);

  decoder->read_scanlines(*img);

  return img;
}
