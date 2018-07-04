/* hdstring.h - holds a string in an object class

the class is inherited from object, so that the string can be inserted into
a container

http://vividos.home.pages.de - vividos@asamnet.de
(c) 1999 Michael Fink
*/

#ifndef __hdstring_h_
#define __hdstring_h_

#include "defs.h"

class hdstring:public object {
  char *str;
 public:
  hdstring(char *s);
  ~hdstring();
  char *getstr(){ return str; };
};


#endif
