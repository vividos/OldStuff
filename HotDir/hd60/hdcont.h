/* hdcont.h - the hotdir file container

the hdfilecontainer contains objects of the class hdfileinfo, which contains
infos about a particular file on disk. the container is a sortable_container,
that means the container itself is sorted by the 'sortCriteria sc'

http://vividos.home.pages.de - vividos@asamnet.de
(c) 1999 Michael Fink
*/

#ifndef __hdcont_h_
#define __hdcont_h_

#include "defs.h"
#include "sortcont.h"
#include "hd60.h"

 // verschiedene Attribute, die ein File annehmen kann
enum fileattr {
  faNormal=0x0, faReadonly=0x1,faHidden=0x2,faSystem=0x4,
  faLabel=0x8,faDir=0x10,faArchive=0x20,faAnyFile=0x3f
};

class hdfileinfo:public sortable_object {
  char *name;
  ulong size;
  word attr;
  word time,date;
 public:
  hdfileinfo(char*fname,ulong fsize,word fattr,word ftime,word fdate);
  ~hdfileinfo();

  char *getName(){ return name; };
  char *getExt();
  ulong getSize(){ return size; };
  word getAttr(){ return attr; };
  word getTime(){ return time; };
  word getDate(){ return date; };

  bool is_bigger_than(sortable_object *so);
};

class hdfilecontainer:public sorted_container {
  char *filespec;    // the pathname
  ulong clustersize; // the clustersize of the drive the filespec points to

  ulong true_size;   // the added size of the files in bytes
  ulong needed_size; // the real amount of bytes needed on disk
 public:
  hdfilecontainer(char*fspec,sortCriteria sc,bool sort_reverse); // constructor
  virtual ~hdfilecontainer();                  // destructor

  void fill_container(); // fills the container with the hdfileinfo's

  ulong get_true_size(){ return true_size; };
  ulong get_needed_size(){ return needed_size; };
};

#endif
