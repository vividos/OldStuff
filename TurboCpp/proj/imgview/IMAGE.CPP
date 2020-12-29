// image.cpp - image class
// (c) 2020 Michael Fink
// https://github.com/vividos/OldStuff/

#include "image.h"
#include <stdio.h>
#include "imgload.h"

image::image(unsigned int width, unsigned int height,
  unsigned char bits_per_pixel, bool has_palette)
  :xres(width),
  yres(height),
  palette(NULL)
{
  const size_t pixel_width = has_palette ? 1 : (bits_per_pixel >> 3);
  data = new byte[(size_t)width * height * pixel_width];

  if (has_palette)
    palette = new byte[256 * 3];
}

image* image::load(const char* _filename)
{
  FILE* fd = fopen(_filename, "rb");
  image_loader* loader = image_loader::get_image_loader(_filename, fd);

  image* img = NULL;
  if (loader != NULL)
    img = loader->load();

  delete loader;

  fclose(fd);

  return img;
}
