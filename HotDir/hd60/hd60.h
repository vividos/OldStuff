/* hd60.h - the hotdir main class

the hotdir class collects all arguments and options, shows the help, and
passes the infos and args to the iterator class hditerator (see 'hditer.h')

http://vividos.home.pages.de - vividos@asamnet.de
(c) 1999 Michael Fink
*/

#ifndef __hd60_h_
#define __hd60_h_

#include "defs.h"
#include "containr.h"
#include "hdcolors.h"

typedef enum { // die verschiedenen Kriterien zum Sortieren
  sortNothing=0,sortByName,sortByExtension,sortBySize,
  sortByDate,sortByTime
} sortCriteria;

struct hdprefs { // die Einstellungen von HotDir
  byte columns;      // Spaltenanzahl
  sortCriteria sc;   // Sortierkriterium
  bool sort_reverse; // Sortierrichtung umdrehen?
  bool linebyline;   // Darstellung line by line?
  bool showEmpty;    // true: zeigt leere container
  hdcolors *col;     // die Klasse liefert Farben je nach Extension
  bool recurse;      // sollen Unterverzeichnisse auch mit angezeigt werden?
  bool treemode;     // sollen die Trees und DiskUsages angezeigt werden?
};

class hotdir:public container { // die HotDir-Haupt-Klasse
  bool help_showed; // wurde die Hilfe angezeigt?
  hdprefs hdp;      // die Einstellungen
 public:
  hotdir(char *exename); // Konstruktor, nimmt argv[0] entgegen
  ~hotdir(); // Destruktor

  void argument(char *arg); // nimmt Argument entgegen

  void start(); // Haupt-Funktion

 protected:
  void show_help(); // zeigt Hilfe
  void extend_path(char *path); // erweitert die Pfadangabe
  void recurse_subdirs(); // holt alle Unterverzeichnisse
  void get_next_subdirs(); // gehoert zu recurse_subdirs()
};

#endif
