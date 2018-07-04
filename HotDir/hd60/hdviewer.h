/* hdviewer.h - shows the hotdir directory

the class takes the hdprefs from hditerator (see 'hditer.h') and view()
shows the contents of the hdfilecontainer, according to the options in
the hdprefs

http://vividos.home.pages.de - vividos@asamnet.de
(c) 1999 Michael Fink
*/

#ifndef __hdviewer_h_
#define __hdviewer_h_

#include "defs.h"
#include "hdcont.h"
#include "hd60.h"

class hdviewer:public object {
  hdprefs *hdp;
 public:
  hdviewer(hdprefs *_hdp);

  void view(hdfilecontainer *hdfc);

 protected:
  void view_lineordered(hdfilecontainer *hdfc);
  void view_columnordered(hdfilecontainer *hdfc);

  void show_entry(hdfileinfo *hdfi,byte column);
  void show_size(ulong size,word attr,bool shortformat);
  void show_date(word date,bool shortformat);
  void show_time(word time,bool shortformat);
  void show_attr(word attr);
};

#endif
