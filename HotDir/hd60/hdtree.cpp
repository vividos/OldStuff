/* hdtree.cpp - shows directory tree

see 'hdtree.h' for details

http://vividos.home.pages.de - vividos@asamnet.de
(c) 1999 Michael Fink
*/

#include "hdtree.h"

#include "formatul.h"
#include "console.h"

#include <string.h>
#include <dir.h>

extern console *con;

treeEntry::treeEntry(char *_name,word depth){
  name=new char[strlen(_name)];
  if (name!=NULL)
    strcpy(name,_name);
  level=depth;
};

treeEntry::~treeEntry(){
  if (name!=NULL) delete name;
};

treeViewer::treeViewer(){
  con->printf("^4Hot^7Tree & ^2Disk^7Usage v1.0^7\n");
  path=NULL;
};

treeViewer::~treeViewer(){
  if (path!=NULL)
    delete path;
};

void treeViewer::processTree(char *_path){
  con->printf("^fPath:^e%s\n",_path);
  if (strrchr(_path,'\\')==NULL) return;

  if (path!=NULL) delete path;
  path=new char[1024];
  strcpy(path,_path);

  move(0,true);
  while (!is_empty()) del();

  paths=0;
  depth=0;
  total=retrieveTree();

  insert( new treeEntry("null",0) );
};

ulong treeViewer::retrieveTree(){
  depth++;
  ffblk ff;
  ulong size=0;

  *(strrchr(path,'\\')+1)=0;
  strcat(path,"*.*");

  if (findfirst(path,&ff,0x3f)==0)
  do {
    if ((ff.ff_attrib&0x10)==0x10){
      if (ff.ff_name[0]!='.'){

	treeEntry *te=new treeEntry(ff.ff_name,depth);
	insert(te);
	paths++;

	*(strrchr(path,'\\')+1)=0;
	strcat(path,ff.ff_name);
	strcat(path,"\\*.*");

	ulong asize=retrieveTree();
	te->setSize(asize);

	if ( (ff.ff_attrib&(0xc0|0x10|0x80))==0 )
	  size+=asize;

	*(strrchr(path,'\\'))=0;
	*(strrchr(path,'\\')+1)=0;
	strcat(path,"*.*");
      };
    } else {
      size+=ff.ff_fsize;
    };

  } while (findnext(&ff)==0);

  depth--;
  return size;
};

void treeViewer::showTree(){
  char help[32];

  treeEntry *te;

  word max=getNumber()-1;

  for(int z=0;z<max;z++){
    move(z,true);
    te=(treeEntry*)getObject();

    if (te->getName()==NULL) break;

    con->printf("^9");
    int i;
    for(i=1;i<te->getLevel();i++)
      con->printf("%c",searchForNextNr(i,z)?'³':' ');

    con->printf("%c^7%s",searchForNextNr(i,z)?'Ã':'À',te->getName());

    word length=strlen(te->getName())+i;

    con->printf("%*c",16-(length%16),' ');

    format_ulong(te->getSize(),help);
    con->printf("^e %16s^7\n",help);
  };

  format_ulong(total,help);
  con->printf("^c%u^a directories, total ^c%s^a bytes.^7\n",paths,help);
};

bool treeViewer::searchForNextNr(word nr,word from){
  move(from,true);

  treeEntry *te;
  bool ende=false;

  do {
    move(1);
    te=(treeEntry*)getObject();

    if (te->getLevel()==nr)
      return true;
    ende=is_at_end();
  } while(!ende&&te->getLevel()>=nr);

  return false;
};
