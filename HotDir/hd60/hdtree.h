/* tree.h - shows directory tree

the treeViewer class acts as the ancient dos program "tree.com", it shows
all subdirectories as a tree; additionally the unix command "du" (for disk
usage) is built in. pass the path to processTree() and show the results with
showTree()

http://vividos.home.pages.de - vividos@asamnet.de
(c) 1999 Michael Fink
*/

#ifndef __tree_h_
#define __tree_h_

#include "defs.h"
#include "containr.h"

// the treeEntry class holds information for one particular path, his size
// including subdirs' size and the level above the current root dir

class treeEntry:public object {
  char *name;
  word level;
  ulong size;
 public:
  treeEntry(char *_name,word depth);
  virtual ~treeEntry();

  void setSize(ulong s){ size=s; };

  char *getName(){ return name; };
  word getLevel(){ return level; };
  ulong getSize(){ return size; };
};

// the treeViewer class holds the treeEntry's, which are collected by
// processTree() and showed by showTree();

class treeViewer:public container {
  char *path;
  word depth;
  word paths;
  ulong total;
 public:
  treeViewer();
  virtual ~treeViewer();
  void processTree(char *_path);
  void showTree();
 private:
  ulong retrieveTree();
  bool searchForNextNr(word nr,word from);
};

#endif
