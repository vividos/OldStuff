/* hd60.cpp - the hotdir main class

see 'hd60.h' for details

http://vividos.home.pages.de - vividos@asamnet.de
(c) 1999 Michael Fink
*/

#include "hd60.h"

#include "console.h"
#include "hdstring.h"
#include "hditer.h"
#include "hdcont.h"

#include <string.h>
#include <ctype.h>
#include <dir.h>

// fuer alle Module, die was ausgeben wollen: die console-Klasse
extern console *con=NULL;
// nur fr win32-Konsolenanwendungen: der Name des Fensters
extern char *console_window_name="HotDir v6.0"/*" beta 16"*/;

hotdir::hotdir(char *exename){ // Konstruktor
  con=new console; // neue console-Klasse, zum Ausgeben
  con->set_fullscreen_text(" [hit a key!]"); // der 'more'-text
  con->printf("^4Hot^7Dir v6.0"/*" ^1beta 16"*/"^7\n"); // HotDir-Meldung

  help_showed=false; // die einzelnen Programmeinstellungen
  hdp.columns=4;
  hdp.sc=sortByExtension;
  hdp.sort_reverse=false;
  hdp.linebyline=true;
  hdp.showEmpty=false;
  hdp.recurse=false;
  hdp.treemode=false;

  char *cfgname=new char[strlen(exename)+2]; // aus dem exe-filenamen den
  strcpy(cfgname,exename);                   // cfg-namen generieren
  (*strrchr(cfgname,'.'))=0;
  strcat(cfgname,".cfg");

  hdp.col=new hdcolors(cfgname); // und damit die hdcolors-Klasse erzeugen
  delete cfgname;                // um das .cfg-File einzulesen
};

hotdir::~hotdir(){ // Destruktor
  delete hdp.col; // weg mit der hdcolors-Klasse
  delete con;     // weg mit der console
};

void hotdir::argument(char *arg){ // nimmt Kommandozeilenargumente entgegen
  if (arg[0]=='-'||arg[0]=='/'){ // wenns mit - oder / anf„ngt ...

    switch(tolower(arg[1])){ // n„chsten Buchstaben untersuchen
      case 'h':
      case '?':
	show_help(); // zeigt Hilfe
	help_showed=true; // verhindert weitere Ausgaben in start()
	break;
      case 'c': // Spalteneinstellung
	hdp.columns=(byte)(arg[2]-0x30); // welche Anzahl?
	if (hdp.columns==0||hdp.columns>6) hdp.columns=4; // im Bereich?
	break;
      case 's': { // Sortieren:
	for(int i=2;arg[i]!=0;i++) // die n„chsten chars durchgehen
	  switch(arg[i]){ // wenn ...
	    case 'n': hdp.sc=sortByName; break; // n=name
	    case 'e': hdp.sc=sortByExtension; break; // e=extension
	    case 's': hdp.sc=sortBySize; break; // s=size
	    case 'd': hdp.sc=sortByDate; break; // d=date
	    case 't': hdp.sc=sortByTime; break; // t=time
	    case '-': hdp.sort_reverse=true; break; // - dreht Reihenfolge um
	  };
	break; };
      case 'u': // up-down-Darstellung ein
	hdp.linebyline=false;
	break;
      case 'e': // leere Verzeichnisse anzeigen ein
	hdp.showEmpty=true;
	break;
      case 'p': // disabled den scree-full-handler der console-Klasse
	con->no_fullscreen_stop();
	break;
      case 'r': // recurse sub-dirs an, gleichzeitig treemode aus
	hdp.recurse=true;
	hdp.treemode=false;
	break;
      case 't': // treemode an, gleichzeitig recuse off
	hdp.treemode=true;
	hdp.recurse=false;
	break;
      default: // meldung bei falschem Arg
	con->printf("option [%s] not recognized\n",arg);
    };

  } else { // kein - oder / als erster char

    char help[256];   // help-string
    strcpy(help,arg);

    extend_path(help); // erweitert den Pfad

    container::insert( new hdstring(help) ); // insertet in den Container
  };
};

void hotdir::start(){ // Haupt-Funktion
  if (help_showed) return; // wenn schon die Hilfe angezeit wurde: Ende

  if (getNumber()==0) // wenn noch keine Argumente in den Container sind:
    argument("*.*"); // Standard-"*.*" einfgen

  if (hdp.recurse) // wenn die recurse subdirs Option an ist:
    recurse_subdirs(); // alle Unterverzeichnisse holen

  hditerator *hdi=new hditerator(&hdp); // neuen Iterator, der alle
  container::iterate(hdi); // Kommandozeilen-Argumente durchgeht
  delete hdi;
};

void hotdir::show_help(){ // zeigt die Hilfe
  con->printf("^2Help:^7\n"
    "Syntax: HD ^e[-options] ^9[drive:]^a[path\]^c[files]^7\n"
    "Options:\n"
    " ^e-h -?  ^7  shows this help you are reading now\n"
    " ^e-c^fX    ^7  uses ^fX^7 columns to show the directory\n"
    " ^e-u     ^7  displays directory column-ordered instead of line-ordered\n"
    " ^e-e     ^7  shows directories with no files\n"
    " ^e-p     ^7  doesn't pause when the screen is full\n"
    " ^e-r     ^7  recursively shows all sub-directories\n"
    " ^e-s^7[^f-^7]^fX   ^7sorts the directory, where ^fX^7 is one of these:\n"
    "          ^fn^7=name, ^fe^7=extension, ^fs^7=size, ^fd^7=date and "
    "^ft^7=time\n"
    "          ^7the optional ^f-^7 reverses the sort order\n"
    " ^e-t     ^7  shows the tree and the disk usage instead of the dir\n"
    "\n(c) 1999 Michael Fink\n"
    "^ahttp://vividos.home.pages.de^7 - ^9vividos@asamnet.de^7\n"
    "HotDir is distrubuted under the ^fGPL^7. See ^fCopying^7 for details.\n"
  );
};

void hotdir::extend_path(char *path){ // erweitert den Pfad
  if (path[0]=='\\'&&path[1]=='\\') // wenn ein Netzwerk-Name angegeben wurde
    return; // z.B. "//Vividos/*.*", dann nix machen

  for(int n=strlen(path);n>=0;n--) // alle chars durchgehen und Linux-'/'
    if (path[n]=='/') path[n]='\\'; // durch popelige dos-'\' ersetzen

  if (strchr(path,':')==0){ // wenn kein Doppelpunkt im String
    for(int i=strlen(path)+2;i>=2;+i--) // 2 chars am Anfang einfgen
      path[i]=path[i-2];
    path[0]=(char)(getdisk()+'a'); // das aktuelle drive holen
    path[1]=':'; // und davor reinschreiben
  };

  if (path[1]==':'&&path[2]==0) // wenn der path nur aus "<drive>:" besteht,
    strcat(path,"\\"); // z.B. "c:", dann ein '\' anfgen

  if (path[strlen(path)-1]=='\\') // wenn als letztes Zeichen ein '\' ist,
    strcat(path,"*.*"); // dann ein "*.*" anh„ngen
  else { // ansonsten
    ffblk ff;
    if (findfirst(path,&ff,faAnyFile)==0) // schauen, ob die Angabe in path
      if (findnext(&ff)!=0)               // nur 1x vorhanden ist, z.B. ein
	if ((ff.ff_attrib&faDir)==faDir){ // Verzeichnis (z.B."c:\windows")
	  strcat(path,"\\*.*");           // dann ein "\*.*" anfgen
	};
  };

  if (path[2]!='\\'){ // wenn das 3. Zeichen kein '\' ist, dann ...
    char curdir[256]; // aktuelles Verzeichnis rausbekommen
    if (getcurdir(path[0]&0x1f,curdir)==0){
      if (curdir[0]!=0&&curdir[strlen(curdir)-1]!='\\')
	strcat(curdir,"\\");
      strcat(curdir,path+2); // pfad an curdir anh„ngen
      strcpy(path+3,curdir); // und ber pfad wieder drberkopieren
      path[2]='\\';
    };
  };
};

void hotdir::recurse_subdirs(){ // holt alle subdirs
  move(0,true); // an den Anfang der Liste
  int cnt=0; // z„hlt die schon bearbeiteten entries der Liste durch
  do {
    get_next_subdirs(); // hole vom aktuellen Listenentry alle subdirs
    cnt++; // zaehle mit
  } while (cnt<=getNumber());  // solange noch nicht alle entries
};

void hotdir::get_next_subdirs(){ // holt alle subdirs des aktuellen entries
  hdstring *hds=(hdstring*)getObject(); // hole String

  char path[256];
  char fspec[32];

  strcpy(path,hds->getstr()); // kopiert den String in path

  char *pos=strrchr(path,'\\')+1;
  strcpy(fspec,pos); // sichert das 'filespec' (z.B. "*.exe")

  *pos=0;
  strcat(path,"*.*"); // fgt ein "*.*" an

  int cnt=0; // zaehlt durch, wieviele entries man am Ende zurckgehen muss
  ffblk ff;

  if (findfirst(path,&ff,0x3f)==0) // wenn das erste gefunden ...
  do {
    if ((ff.ff_attrib&0x10)==0x10&&ff.ff_name[0]!='.'){ // und kein . oder ..
      *(strrchr(path,'\\')+1)=0; // weg mit dem filespec
      strcat(path,ff.ff_name); // fuegt gefundenen Namen und ein '\' an
      strcat(path,"\\");
      strcat(path,fspec); // und das filespec
      container::insert( new hdstring(path) ); // und insertet das
      *(strrchr(path,'\\'))=0; // dann wieder filespec weg
      cnt++; // mitzaehlen
    };
  } while (findnext(&ff)==0); // das ganze, solange noch files gefunden werden
  move(1-cnt); // moved wieder zurueck, auf den urspruenglichen entry + 1
};
