/* sortcont.h - a container with sorted content

see 'sortcont.h' for details

http://vividos.home.pages.de - vividos@asamnet.de
(c) 1998-1999 Michael Fink
*/

#ifndef __sortcont_h_
#define __sortcont_h_

#include "defs.h"
#include "containr.h"

class sortable_object:public object { // the sortable object class
 public:
  sortable_object(); // constructor
  virtual bool is_bigger_than(sortable_object *so);
    // compares if this object is "bigger" than 'so'
};

class sorted_container:public container { // the sorted container
 public:
  sorted_container(); // constructor
  void sorted_insert(sortable_object *so); // inserts the object to sort
};

#endif
