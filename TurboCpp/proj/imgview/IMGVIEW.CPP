// imgview.cpp - image viewer application
// (c) 2020 Michael Fink
// https://github.com/vividos/OldStuff/
// Image Viewer is an application to display images using VGA or SVGA
// screens. It was written in 2020 but contains many components originally
// written in 1998/1999.

#include <stdio.h>
#include <sys/stat.h>
#include "imgview.h"
#include "imgload.h"
#include "graphic.h"

image_viewer::image_viewer()
:image_filename(260),
starting_directory(260)
{
  printf("Image Viewer v1.0\n");
  printf("\n");
}

image_viewer::~image_viewer()
{
  if (graphics_mode != NULL)
  {
    graphics_mode->off();
    sc.restore();
  }

  delete graphics_mode;
}

void image_viewer::arg_option(const string& arg)
{
  // is it an option?
  if (arg.length() > 1 &&
      arg.get(0) == '-')
  {
    string option(
      arg.get(1) == '-' ? arg.getstr(2) : arg.getstr(1));

    option.lowercase();
    if (option == "h" || option == "?" || option == "help")
    {
      show_help();
      ende = true;
      return;
    }

    printf("Unknown command line arg specified: %s\n", arg.getstr());
  }
  else
  {
    // no option; check if it's a directory or a file
    string path(arg);

    struct stat statbuf = { 0 };
    if (0 == stat(path.getstr(), &statbuf))
    {
      if ((statbuf.st_mode & S_IFDIR) != 0)
      {
        starting_directory = path;
      }
      else
      {
        image_filename = path;
      }
      return;
    }

    printf("Invalid directory or filename specified: %s", path.getstr());
  }
}

void image_viewer::show_help()
{
  printf("Syntax: imghelp {options} {filename-or-dir}\n"
    "You can specify one image filename to display, or a starting\n"
    "directory where browsing for image files starts.\n"
    "Available options:\n"
    "  -h -? --help   Shows this help\n"
    "\n");
}

void image_viewer::init()
{
  if (image_filename.length() == 0)
  {
    // TODO browse for a file
    printf("TODO: Browsing for files!\n");
  }

  if (image_filename.length() == 0)
  {
    ende = true;
    return;
  }

  if (!load_and_show())
    ende = true;
}

bool image_viewer::load_and_show()
{
  // open file and get an image loader
  FILE* fd = fopen(image_filename.getstr(), "rb");
  image_loader* loader =
    image_loader::get_image_loader(image_filename, fd);
  if (loader == NULL)
  {
    printf("Couldn't find image loader for filename: %s\n",
      image_filename.getstr());
    fclose(fd);
    return false;
  }

  word xres = 0, yres = 0;
  byte bits_per_pixel = 0;
  bool has_palette = false;
  byte palette[256][3] = { 0 };

  bool ret = loader->read_metadata(xres, yres,
    bits_per_pixel, has_palette, palette);
  if (!ret)
  {
    printf("Couldn't read image metadata for filename: %s\n",
      image_filename.getstr());
    delete loader;
    fclose(fd);
    return false;
  }

  // initialize graphics by finding proper video mode
  graphics* gr = graphics::find_mode(
    xres, yres,
    has_palette ? 8 : 24,
    false);

  if (gr == NULL)
  {
    printf("Couldn't find proper video mode for image with size %ix%ix%i\n",
      xres, yres, has_palette ? 8 : 24);
    delete loader;
    fclose(fd);
    return false;
  }

  graphics_mode = gr;

  // transfer image to graphics
  sc.store();
  gr->on();

  if (has_palette)
    gr->set_palette(palette);

  word xoffset = (gr->get_xres() - xres) / 2;
  word yoffset = (gr->get_yres() - yres) / 2;

  word scanline_length = xres * (has_palette ? 1 : 3);

  int y;
  do
  {
    y = loader->get_next_y();
    if (y < 0)
      break;

    byte far* scanline = gr->get_framebuffer_ptr(xoffset, y + yoffset);

    bool ret2 = loader->read_scanline(scanline, scanline_length);

    if (!ret2)
    {
      gr->off();
      sc.restore();
      printf("Error reading scanline #%i\n", y);
      ret = false;
      break;
    }

  } while (y >= 0);

  delete loader;
  fclose(fd);

  return ret;
}

void image_viewer::handle_key(keycode kc)
{
  if (kc == kbEsc)
    ende = true;
}

// main entry point
void main(int argc, char** argv)
{
  image_viewer iv;
  iv.run(argc, argv);
};
