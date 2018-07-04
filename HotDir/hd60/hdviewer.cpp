/* hdviewer.cpp - shows the hotdir directory

see 'hdviewer.h' for details

http://vividos.home.pages.de - vividos@asamnet.de
(c) 1999 Michael Fink
*/

#include "hdviewer.h"
#include "formatul.h"
#include "console.h"

#include <stdlib.h>
#include <string.h>

extern console *con;

hdviewer::hdviewer(hdprefs *_hdp){
  hdp=_hdp;
};

void hdviewer::view(hdfilecontainer *hdfc){
  if (hdp->columns==1||hdfc->getNumber()<=hdp->columns)
    view_lineordered(hdfc);
  else {
    if (hdp->linebyline)
      view_lineordered(hdfc);
    else
      view_columnordered(hdfc);
  };
};

void hdviewer::view_lineordered(hdfilecontainer *hdfc){
  hdfc->move(0,true);

  word allentrys=hdfc->getNumber();

  word entry=0;
  do {
    hdfileinfo *hdfi=(hdfileinfo*)hdfc->getObject();
    show_entry(hdfi,(entry++)%hdp->columns);
    hdfc->move(1);

  } while (entry<allentrys);

  if ((allentrys%hdp->columns)!=0)
    con->printf("^7\n");
};

void hdviewer::view_columnordered(hdfilecontainer *hdfc){
  word columns=hdp->columns;

  word allentrys=hdfc->getNumber();

  word colentrys=allentrys;
  if (colentrys%columns!=0)
    colentrys+=columns-(colentrys%columns);
  colentrys/=columns;

  word biswo=colentrys-
    (allentrys%columns==0?0:columns-allentrys%columns);

  for(int n=0;n<columns;n++){
    hdfc->move(n*colentrys,true);
    hdfc->set_marker_here(n);
  };

  word entry=0;
  do {
    if ( !((entry%columns+1==columns)&&(entry/columns>=biswo)) )
    {
      hdfc->jump_to_marker(entry%columns);
      hdfileinfo *hdfi=(hdfileinfo*)hdfc->getObject();
      show_entry(hdfi,entry%columns);
      hdfc->move(1);
      hdfc->set_marker_here(entry%columns);
    } else
      con->printf("\n");
    entry++;

  } while (entry<colentrys*columns);
};

void hdviewer::show_entry(hdfileinfo *hdfi,byte column){
  byte color=hdp->col->get_color( hdfi->getExt() );

  color+=color<10?'0':'a'-10;

  switch (hdp->columns){
    case 6:
      con->printf("^4³^%c%-12.12s^7",color,hdfi->getName());
      if (column==5) con->printf("^4³\n");
      break;

    case 5:
      con->printf("^4³");
      con->printf("^%c%-15.15s",color,hdfi->getName());
      break;

    case 4:
      con->printf("^4³");
      show_size(hdfi->getSize(),hdfi->getAttr(),true);
      con->printf("^%c%-12.12s",color,hdfi->getName());
      break;

    case 3: // 26
      con->printf("^4³");
      show_attr(hdfi->getAttr());
      show_size(hdfi->getSize(),hdfi->getAttr(),true);
      con->printf("^%c%-12.12s",color,hdfi->getName());
      if (column==2) con->printf("^4³\n");
      break;

    case 2:
      con->printf("^4³");
      show_date(hdfi->getDate(),true);
      show_time(hdfi->getTime(),true);
      show_size(hdfi->getSize(),hdfi->getAttr(),true);
      con->printf("^%c%-17.17s^7",color,hdfi->getName());
      break;

    case 1:
      con->printf("^4³");
      show_date(hdfi->getDate(),false);
      show_time(hdfi->getTime(),false);
      show_attr(hdfi->getAttr());
      con->printf(" ");
      show_size(hdfi->getSize(),hdfi->getAttr(),true);
      con->printf("^%c%-44.44s^7\n",color,hdfi->getName());
      break;
  };
};

// shortformat==true:  7 chars
// shortformat==false: 14 chars
void hdviewer::show_size(ulong size,word attr,bool shortformat){

  if ((attr&faDir)==faDir){
    con->printf("%s",shortformat?"^d <dir> ":"^d <directory>  ");
  } else
  if ((attr&faLabel)==faLabel){
    con->printf("%s",shortformat?"^b[label]":"^b [disk label] ");
  } else
  if ((attr&0xc0)!=0){
    con->printf("%s",shortformat?"^8{win9x}":"^8 {win9x lfn}  ");
  } else {

    char line[34]; strcpy(line,"^e");
    if (shortformat){
      if (size>=1000000L){
	if (size>=100000000L){
	  ultoa(size/1000000L,line+2,10);
	  strcat(line,"M");
	  line[1]='c';
	} else {
	  ultoa(size/1000,line+2,10);
	  strcat(line,"K");
	  line[1]='d';
	};

      } else
	ultoa(size,line+2,10);
      con->printf("%8.8s ",line);
    } else {
      format_ulong(size,line+2);
      con->printf("%15.15s ",line);
    };

  };
};

typedef union {
  struct dos_date {
    byte day:5;
    byte month:4;
    byte year:7;
  } ddate;
  word date;
} sdate;

typedef union {
  struct dos_time {
    byte sec:5; // sec/2
    byte min:6;
    byte hour:5;
  } dtime;
  word time;
} stime;

// shortformat==true:  8 chars
// shortformat==false: 10 chars
void hdviewer::show_date(word date,bool shortformat){
  sdate d; d.date=date;
  con->printf("^a%2.2u.%2.2u.",d.ddate.day,d.ddate.month);
  if (shortformat)
    con->printf("%2.2u",(d.ddate.year+80)%100);
  else
    con->printf("%4.4u",d.ddate.year+1980);
  con->printf(" ");
};

// shortformat==true:  5 chars
// shortformat==false: 8 chars
void hdviewer::show_time(word time,bool shortformat){
  stime t; t.time=time;
  con->printf("^9%2.2u:%2.2u",t.dtime.hour,t.dtime.min);
  if (!shortformat)
    con->printf(":%2.2u",t.dtime.sec*2);
  con->printf(" ");
};

void hdviewer::show_attr(word attr){
  char help[8]; strcpy(help,"----- ");
  if ((attr&faDir)==faDir)
    help[0]='d';
  if ((attr&faReadonly)==faReadonly)
    help[1]='r';
  if ((attr&faHidden)==faHidden)
    help[2]='h';
  if ((attr&faSystem)==faSystem)
    help[3]='s';
  if ((attr&faArchive)==faArchive)
    help[4]='a';
  if ((attr&faLabel)==faLabel)
    help[5]='l';

  con->printf("^7%s",help);
};


