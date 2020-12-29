// imgview.h - image viewer application
// (c) 2020 Michael Fink
// https://github.com/vividos/OldStuff/
// Image Viewer is an application to display images using VGA or SVGA
// screens. It was written in 2020 but contains many components originally
// written in 1998/1999.

#include "app.h"

class graphics;

// image viewer app class
// the image viewer application supports showing an image file directly,
// by loading the image, switching into VGA or SVGA modes and displaying
// the image, or by letting the user browse the current (or specified)
// directory and select an image to display.
class image_viewer: public app
{
public:
  // ctor
  image_viewer();
  // dtor
  virtual ~image_viewer();

  // called to parse command line arguments and options
  virtual void arg_option(const string& arg);

  // initializes app
  virtual void init();

  // handles key presses
  virtual void handle_key(keycode kc);

private:
  // prints the help of the application
  void show_help();

  // loads and shows image
  bool load_and_show();

private:
  // contains the filename of the image to view, if any
  string image_filename;

  // contains the starting directory for browsing for files
  string starting_directory;

  // graphics mode to use to display image
  graphics* graphics_mode;
};
