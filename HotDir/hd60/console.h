/* console.h - console functions

the module provides functions for the output to the console screen. change
the code for porting to other platforms. provided here is "doscon.cpp" for
the dos 16-bit console (as in ANSI C) and "w32con.cpp" for the 32-bit
console (the "kernel.lib" must be linked). probably a linux console module
may follow.

http://vividos.home.pages.de - vividos@asamnet.de
(c) 1999 Michael Fink
*/

#ifndef __console_h_
#define __console_h_

#include "defs.h"

extern char*console_window_name;

class console:public object {
  bool hold_at_fullscreen;
  int printed_lines;
  char *fullscreen_text;
 public:
  console();
  void set_fullscreen_text(char*text){ fullscreen_text=text; };
  void no_fullscreen_stop(){ hold_at_fullscreen=false; };

  virtual void setcolor(byte color);
  virtual void printchar(char c);
  virtual int wherex();
  virtual void gotoxy(int x,int y);
  virtual void clreol();
  int printf(char*fmt,...);
 private:
  void new_line_reached();
};

#endif
