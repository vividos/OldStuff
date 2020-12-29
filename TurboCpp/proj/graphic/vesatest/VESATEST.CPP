// vesatest.cpp - VESA graphics mode test application
// (c) 1997, 2020 Michael Fink
// https://github.com/vividos/OldStuff/

#include <stdio.h>
#include <conio.h>
#include "vesamode.h"

void main()
{
  printf("VESA graphics mode test\n\n");

  vesa_mode::show_modes();
  getch();
};
