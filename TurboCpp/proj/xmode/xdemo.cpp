// xdemo.cpp - xdemo Mode X demo program
// (c) 1999 Michael Fink
// https://github.com/vividos/OldStuff/

#include "xmode.h"

#include <conio.h>

void main()
{
  xmode x;

  x.on();
  x.clearpage(2);
  x.setpage(0);
  x.showpage(0);
  x.pagemove(2);

  x.pagemove(x.activepage());

  byte array[16] = { 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15 };

  for (int i = 0; i < 100; i++)
    x.setline(i, i, array, 16);

  getch();

  x.off();
};
