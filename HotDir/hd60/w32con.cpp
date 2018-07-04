/* w32con.cpp - win32 console functions

see 'console.h' for details

http://vividos.home.pages.de - vividos@asamnet.de
(c) 1999 Michael Fink
*/

#include "console.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#include <windows.h>
#include <wincon.h>

HANDLE win32screenhandle;
word win32xpos;
byte win32color;

extern char *console_window_name;

console::console(){
  SetConsoleTitle(console_window_name);

  win32screenhandle=GetStdHandle(STD_OUTPUT_HANDLE);

  CONSOLE_SCREEN_BUFFER_INFO csbi;
  GetConsoleScreenBufferInfo(win32screenhandle,&csbi);
  win32xpos=csbi.dwCursorPosition.X;

//  ulong conMode;
//  GetConsoleMode(win32screenhandle,&conMode);

//  SetConsoleMode(win32screenhandle,!ENABLE_LINE_INPUT);

//  win32emptybuffer();
//  win32cnt=0;

  hold_at_fullscreen=true;
  printed_lines=0;
  fullscreen_text=NULL;
};

void console::setcolor(byte color){
//  SetConsoleTextAttribute(win32screenhandle,color);
  win32color=color;
};

void console::printchar(char c){
  char *from="\xe4\xf6\xfc\xc4\xd6\xdc\xdf";
  char *to  ="\x84\x94\x81\x8e\x99\x9a\xe1";

  for(int i=strlen(from);i>=0;i--)
    if (c==from[i])
      c=to[i];

  if ( c=='\n'){
    SetConsoleTextAttribute(win32screenhandle,0x07);
  };

  SetConsoleTextAttribute(win32screenhandle,win32color);

  DWORD dummy=0;
  LPVOID dummy2=NULL;
  WriteConsole(win32screenhandle,(CONST VOID*)&c,1,&dummy,dummy2);
  win32xpos++;

  if ( c=='\n' || win32xpos==80 ){
    win32xpos=0;
//    win32writebuffer();
    SetConsoleTextAttribute(win32screenhandle,win32color);
    new_line_reached();
  };

//  if (c=='\n'||c=='\r'||win32xpos==80)
//    SetConsoleTextAttribute(win32screenhandle,0x07);
/*

  SetConsoleTextAttribute(win32screenhandle,win32color);

  DWORD dummy=0;
  LPVOID dummy2=NULL;
  WriteConsole(win32screenhandle,(CONST VOID*)&c,1,&dummy,dummy2);
  win32xpos++;

  if (win32xpos==80||c=='\n'){
    win32xpos=0;
    new_line_reached();
  };

  if (c=='\n'||c=='\r')
    SetConsoleTextAttribute(win32screenhandle,win32color);*/
};
/*
int win32getch(){
  char b[2]; b[0]=0;
  ulong dummy1=0;
  FlushConsoleInputBuffer(win32screenhandle);
  do
    ReadConsole(win32screenhandle,(LPVOID)b,1,&dummy1,NULL);
  while (b[0]==0&&b[1]==0);
  ::printf("char was: 0x%2x%2x\n",b[0],b[1]);
  return b[0];
};
*/

void console::new_line_reached(){
  if (hold_at_fullscreen&&++printed_lines==24){
    gotoxy(1,25); clreol();
    ::printf(fullscreen_text);
    word key;
    getc(stdin); // geht net anders :-/
    if (key==27) exit(0);
    gotoxy(1,25); clreol();
    printed_lines=0;
  };
};

int console::wherex(){
  return win32xpos;
};

void console::gotoxy(int x,int y){
//  ::gotoxy(x,y);
  COORD c={x,y};
  SetConsoleCursorPosition(win32screenhandle,c);
};

void console::clreol(){
//  ::clreol();
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

