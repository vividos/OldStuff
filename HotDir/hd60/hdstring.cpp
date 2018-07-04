/* hdstring.cpp - holds a string in an object class

see 'hdstring.h' for details

http://vividos.home.pages.de - vividos@asamnet.de
(c) 1999 Michael Fink
*/

#include "hdstring.h"

#include <string.h>

hdstring::hdstring(char *s){
  if (s!=NULL){
    str=new char[strlen(s)+2];
    if (str!=NULL)
      strcpy(str,s);
  };
};

hdstring::~hdstring(){
  if (str!=NULL)
    delete str;
};
