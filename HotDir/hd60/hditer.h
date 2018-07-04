/* hditer.h - iterates through the options of hotdir

takes the hdprefs from the hotdir class and iterates through all the paths
from the commandline inside the container hotdir; additionally it outputs
the info about the files showed (e.g. the total size), and infos about the
drives used

http://vividos.home.pages.de - vividos@asamnet.de
(c) 1999 Michael Fink
*/

#ifndef __hditer_h_
#define __hditer_h_

#include "defs.h"
#include "containr.h"
#include "hd60.h"

class hditerator:public iterator {
  hdprefs *hdp;   // the hotdir preferences from the class hotdir
  word pathcount; // how much paths were showed?
  bool *drives;   // bool array; which drive was used?

  ulong filecount;       // how much files were showed allover?
  ulong all_needed_size; // how many bytes were in these files?
  ulong all_true_size;   // which physical space on the disk used they?
 public:
  hditerator(hdprefs *_hdp); // constructor
  ~hditerator();             // destructor

  virtual void iterate(object *o); // takes all the pathspecs
 protected:
  void show_allpath_infos();         // shows infos when pathcount < 1
  void show_drive_infos(char drive); // shows infos about drive 'drive'
};

#endif
