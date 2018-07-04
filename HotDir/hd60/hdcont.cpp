/* hdcont.cpp - the hotdir file container

see 'hdcont.h' for details

http://vividos.home.pages.de - vividos@asamnet.de
(c) 1999 Michael Fink
*/

#include "hdcont.h"

#include <string.h>
#include <dir.h>
#include <dos.h>

sortCriteria hdfc_sort_by; // internal local variable, keeps the sortCriteria
bool hdfc_sort_reverse;

hdfileinfo::hdfileinfo(char*fname,ulong fsize,word fattr,word ftime,
word fdate){
  if (fname!=NULL){
    name=new char[strlen(fname)+2];
    if (name!=NULL)
      strcpy(name,fname);

    size=fsize; attr=fattr; time=ftime; date=fdate;
  };
};

hdfileinfo::~hdfileinfo(){
  if (name!=NULL)
    delete name;
};

char *hdfileinfo::getExt(){
  char *where=strrchr(name,'.');
  return where==NULL?name+strlen(name):where+1;
};

bool hdfileinfo::is_bigger_than(sortable_object *so){
  hdfileinfo *fi=(hdfileinfo*)so;

  bool result=false;

  bool isdir1=(getAttr()&faDir)==faDir;
  bool isdir2=(fi->getAttr()&faDir)==faDir;

  if (isdir1||isdir2){

    if (isdir1&&isdir2)
      return (bool)(strcmp(name,fi->getName())>0);
    else
      return isdir2;

  } else
    switch(hdfc_sort_by){
      case sortNothing:
	result=false; break;

      case sortByName:
	result=(bool)(strcmp(name,fi->getName())>0); break;

      case sortByExtension: {
	char *ext1=strrchr(name,'.'),*ext2=strrchr(fi->getName(),'.');
	if (ext1==NULL) return false;
	if (ext2==NULL) return true;
	result=(bool)(strcmp(ext1,ext2)>0); break;
      };

      case sortBySize:
	result=(bool)(size>fi->getSize()); break;

      case sortByDate:
	result=(bool)(date>fi->getDate()); break;

      case sortByTime:
	result=(bool)(time>fi->getTime()); break;

      default:;
    };

  return result^hdfc_sort_reverse;
};

hdfilecontainer::hdfilecontainer(char*fspec,sortCriteria sc,bool sort_reverse){
  hdfc_sort_by=sc;
  hdfc_sort_reverse=sort_reverse;
  filespec=new char[strlen(fspec)+2];
  if (filespec!=NULL){
    strcpy(filespec,fspec);
  };

  clustersize=0;

  char drive=(fspec[1]==':')?(fspec[0])&0x1f:getdisk()+1;

  dfree df;
  getdfree(drive,&df);
  clustersize=df.df_bsec*df.df_sclus;
  if (clustersize==0) clustersize=2048;

  true_size=0;
  needed_size=0;
};

hdfilecontainer::~hdfilecontainer(){
  if (filespec!=NULL)
    delete filespec;
};


void hdfilecontainer::fill_container(){
  ffblk *sr=new ffblk;

  if ( findfirst(filespec,sr,0xff)==0 )
  do {
    if ( !((sr->ff_attrib&0xc0)!=0&&sr->ff_fsize==(ulong)-1) ){
    hdfileinfo *hdfi=new hdfileinfo(sr->ff_name,sr->ff_fsize,
      sr->ff_attrib,sr->ff_ftime,sr->ff_fdate);

    sorted_container::sorted_insert( hdfi );

    if ( (sr->ff_attrib&(0xc0|faDir|faLabel))==0 ){

      true_size+=sr->ff_fsize;
      needed_size+=sr->ff_fsize;
      if (sr->ff_fsize%clustersize!=0)
	needed_size+=clustersize-(sr->ff_fsize%clustersize);
      };

    };

  } while(findnext(sr)==0);

  delete sr;
};
