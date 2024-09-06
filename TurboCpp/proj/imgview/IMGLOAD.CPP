// imgload.cpp - image loader
// (c) 2020,2021 Michael Fink
// https://github.com/vividos/OldStuff/

#include "imgload.h"
#include "gifload.h"
#include "jpegload.h"
#include "pngload.h"
#include "cstring.h"

image_loader* image_loader::get_image_loader(const char* _filename, FILE* fd)
{
  string local_filename(_filename);
  int pos = local_filename.firstpos('.', true);
  if (pos == 0)
    return NULL;

  string extension(local_filename.getstr(pos + 1));
  extension.lowercase();

  if (extension == "gif")
  {
    return new gif_loader(fd);
  }
  else if (extension == "jpg")
  {
    return new jpeg_loader(fd);
  }
  else if (extension == "png")
  {
    return new png_loader(fd);
  }

  return NULL;
}
