/* hdcolors.h - manages the colors in hotdir

the class hdcolors is initialized with the filename of the config-file,
where the "extension -->color"-assignment is described. the function
... delivers the proper color # for the given extension

http://vividos.home.pages.de - vividos@asamnet.de
(c) 1999 Michael Fink
*/

#ifndef __hdcolors_h_
#define __hdcolors_h_

#include "defs.h"

 // color codes
enum colors {black=0,blue,green,cyan,red,magenta,brown,
  lightgray,darkgray,lightblue,lightgreen,lightcyan,lightred,
  lightmagenta,yellow,white,blink=128
};

class hdcolors:public object {
  // irgendein array oder so
  char **cols;
 public:
  hdcolors(char *filename);
  ~hdcolors();

  byte get_color(char *ext);

 private:
  void read_config(char *filename);
  void process_line(char *line);
};

#endif
