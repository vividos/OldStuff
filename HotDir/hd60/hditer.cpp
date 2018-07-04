/* hditer.cpp - iterates through the options of hotdir

see 'hditer.h' for details

http://vividos.home.pages.de - vividos@asamnet.de
(c) 1999 Michael Fink
*/

#include "hditer.h"
#include "hdstring.h"
#include "formatul.h"
#include "console.h"
#include "hdcont.h"
#include "hdviewer.h"
#include "hdtree.h"

#include <string.h>
#include <dir.h>
#include <dos.h>

extern console *con;

hditerator::hditerator(hdprefs *_hdp){
  hdp=_hdp;

  drives=new bool[26];
  for(int n=0;n<26;drives[n++]=false);

  pathcount=0;
  filecount=0;
  all_needed_size=0;
  all_true_size=0;
};

hditerator::~hditerator(){
  if (!hdp->treemode){
    if (pathcount==0)
      con->printf("^7sorry, no output.\n");

    if (pathcount>1)
      show_allpath_infos();
  };

  for(int n=0;n<26;n++)
    if (drives[n])
      show_drive_infos( n+'A' );

  delete drives;
};


void hditerator::iterate(object *o){
  hdstring *hds=(hdstring*)o;

  if (hdp->treemode){
    char *name=hds->getstr();
    drives[ (name[1]==':') ? (name[0]-1)&0x1f : getdisk()  ] = true;

    treeViewer *tv=new treeViewer;

    tv->processTree(hds->getstr());
    tv->showTree();

    delete tv;
    return;
  };

   // files in container holen
  hdfilecontainer *hdfc=
    new hdfilecontainer(hds->getstr(),hdp->sc,hdp->sort_reverse);
  hdfc->fill_container();

  if (! (!hdp->showEmpty&&hdfc->getNumber()==0) ) {

    con->printf("^fPath:^e%s^7\n",hds->getstr());
    pathcount++;

    char *name=hds->getstr();
    drives[ (name[1]==':') ? (name[0]-1)&0x1f : getdisk()  ] = true;

    if (hdfc->getNumber()==0){
      con->printf("no files found.\n");
    } else {

      // files anzeigen !!!
      hdviewer *hdv=new hdviewer(hdp);
      hdv->view( hdfc );
      delete hdv;

      ulong needed=hdfc->get_needed_size(),truesize=hdfc->get_true_size();

      char line[32];

      con->printf("^c%u^a files, using ^c%s^a bytes, ",
	hdfc->getNumber(),format_ulong(truesize,line));

      word rate=0;
      if (needed!=0)
	rate=(word)( ((ulong)truesize) / ((ulong)needed/100L) );

      con->printf("needing ^c%s^a bytes (^c%u%%^a).^7\n",
	format_ulong(needed,line),rate);

      filecount+=hdfc->getNumber();
      all_needed_size+=needed;
      all_true_size+=truesize;
    };
  };

  delete hdfc;
};

void hditerator::show_allpath_infos(){
  char line[32];
  con->printf("^4%u^2 paths, ^4%lu^2 files, using ^4%s^2 bytes, ",
      pathcount,filecount,format_ulong(all_true_size,line));

  con->printf("needing ^4%s^2 bytes.^7\n",format_ulong(all_needed_size,line));
};

void hditerator::show_drive_infos(char drive){
  dfree df;
  getdfree(drive&0x1f,&df);

  ulong clsize=(ulong)df.df_bsec*df.df_sclus;
  ulong free=(ulong)df.df_avail*clsize;
  ulong total=(ulong)df.df_total*clsize;

  char line[32];

  con->printf("^adrive ^c%c:^a info: ^c%s^a bytes of ",drive,
    format_ulong(free,line) );

  con->printf("^c%s^a bytes available.^7\n",format_ulong(total,line));

  word rate=0;
  if (total!=0)
    rate=(word)( ((ulong)free) / ((ulong)total/10000L) );

  char path[8];
  strcpy(path,"x:\\*.*");
  path[0]=drive;

  char *label=NULL;
  ffblk ff;

  if (findfirst(path,&ff,FA_LABEL)==0){
    int res=0;
    while ((ff.ff_attrib&0xc0)!=0&&res==0) res=findnext(&ff);
    if (res==0)
      label=ff.ff_name;
  };

  if (label==NULL)
    label="^anone";
  else
    if (strlen(label)>=8)
      for(int n=8;n<13;n++)label[n]=label[n+1];

  con->printf("         ^c%u.%2.2u%%^a free; Label: ^e%.11s^7\n",
    rate/100,rate%100,label);
};


