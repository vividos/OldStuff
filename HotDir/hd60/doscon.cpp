/* doscon.cpp - dos console functions

see 'console.h' for details

http://vividos.home.pages.de - vividos@asamnet.de
(c) 1999 Michael Fink
*/

#include "console.h"
#include <conio.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

/* dos-console-specific --------------------------------------- */

bool doscon_redirected;
int doscon_outchars;

//#define CHECKTEXTMODE

//#ifdef CHECKTEXTMODE
 void check_textmode(){
   byte mode,columns;
   asm { mov ax,0x0f00; int 10h; mov mode,al; mov columns,ah};
   if (mode!=3||columns!=80){
     asm {mov ax,3; int 10h}; gotoxy(0,0);
   };
 };
//#endif

bool wird_redirected(){
  bool ret=false;
  asm { mov ax,4400h; mov bx,1; int 21h };
  if ((_DL&0x82)!=0x82)
    ret=true;
  return ret;
};

/* ------------------------------------------------------------ */


console::console(){
//#ifdef CHECKTEXTMODE
  check_textmode();
//#endif
  doscon_redirected=wird_redirected();
  doscon_outchars=0;
  hold_at_fullscreen=true;
  printed_lines=0;
  fullscreen_text=NULL;
};

void console::setcolor(byte color){
  ::textcolor(color);
};

void console::printchar(char c){
  if (doscon_redirected){
    ::printf("%c",c);
    doscon_outchars++;

    if (c=='\n')
      doscon_outchars=0;

    if (doscon_outchars==80){
      ::printf("%c",'\n');
      doscon_outchars=0;
    };

  } else {
    int lastx=wherex();

    cprintf("%c",c);
    if (c=='\n'){
      cprintf("%c",'\r');
    };

    if (wherex()-lastx!=1)
      new_line_reached();
  };
};

void console::new_line_reached(){
  if (hold_at_fullscreen&&++printed_lines==24){
    gotoxy(1,25);
    clreol();
    ::printf(fullscreen_text);
    word key;
    if ((key=getch())==0) key=getch()<<8;
    if (key==27) exit(0);
    gotoxy(1,25);
    clreol();
    printed_lines=0;
  };
};

int console::wherex(){
  return ::wherex();
};

void console::gotoxy(int x,int y){
  ::gotoxy(x,y);
};

void console::clreol(){
  ::clreol();
};

int console::printf(char*fmt,...){
  va_list argptr;
  int count;
  char buffer[1024];

  va_start(argptr, fmt);
  count = vsprintf(buffer, fmt, argptr);
  va_end(argptr);

  for(int i=0;buffer[i];i++){
    if (buffer[i]=='^'){
      unsigned char color=buffer[++i]&0x5f;
      if (color==0) break;
      color-=color>0x20?55:0x10;
      setcolor(color);
    } else {
      printchar(buffer[i]);
    };
  };

  return count;
};
