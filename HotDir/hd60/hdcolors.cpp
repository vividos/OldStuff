/* hdcolors.cpp - manages the colors in hotdir

see 'hdcolors.h' for details

http://vividos.home.pages.de - vividos@asamnet.de
(c) 1999 Michael Fink
*/

#include "hdcolors.h"

#include "console.h"

#include <stdio.h>
#include <string.h>

extern console *con;

hdcolors::hdcolors(char *filename){
  cols=new char *[16];
  for(int c=0;c<16;c++)
    cols[c]=NULL;

  read_config(filename);
};

hdcolors::~hdcolors(){
  for(int c=0;c<16;c++)
    if (cols[c]!=NULL)
      delete cols[c];
  delete cols;
};

void hdcolors::read_config(char *filename){
  FILE *cfgfile=fopen(filename,"rt");
  if (cfgfile==NULL)
    con->printf("^7failed to open config-file %s\n",filename);
  else {
    char line[256]; line[255]=0;
    while(!feof(cfgfile)){
      fgets(line,255,cfgfile);
      line[strlen(line)!=0?strlen(line)-1:0]=0;
      char *pos;
      pos=strchr(line,';'); if (pos!=NULL) *pos=0;
      pos=strchr(line,'#'); if (pos!=NULL) *pos=0;

      if (*pos!=0){

	pos=line+strlen(line)-1;
	while(*pos==' ')pos--;
	*(++pos)=0;

	pos=line;
	while(*pos==' ')pos++;

//   strupr(pos);
	strlwr(pos);

	if (*pos!=0&&strrchr(pos,'=')!=0)
	  process_line(pos);
      };

    };

    fclose(cfgfile);
  };
};

const char* colnames[16]={"black","blue","green","cyan","red","magenta",
"brown","lightgray","darkgray","lightblue","lightgreen","lightcyan",
"lightred","lightmagenta","yellow","white"};

void hdcolors::process_line(char *line){
//  myprintf("str:[%s]\n",line);

  char *pos=strchr(line,'=')+1;
  *(pos-1)=0;

  if (*pos!=0){

    int col;
    for (col=0;col<16;col++)
      if (strcmp(line,colnames[col])==0)
	break;

    if (col!=16){
      if (cols[col]!=NULL) delete cols[col];
      cols[col]=new char[strlen(pos)+3];
      if (cols[col]!=NULL){
	strcpy(cols[col],pos);
	strcat(cols[col],",");
      };
    };

  };
};

byte hdcolors::get_color(char *ext){
  char hlp[7];
  if (ext!=NULL&&ext[0]!=0&&strlen(ext)<5){
    strcpy(hlp,ext);
    strlwr(hlp);
    for(byte c=0;c<16;c++)
      if (cols[c]!=NULL){
	char *pos=strstr(cols[c],hlp);
	if (pos!=NULL){
	  if (pos[strlen(hlp)]==',')
	    return c;
	};

      };
  };
  return lightgray;
};

