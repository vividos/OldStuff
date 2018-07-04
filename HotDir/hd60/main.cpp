/* main.cpp - main function of hotdir

it passes the arguments to the hotdir class before starting it; if you
compile it with DEBUG_IT defined, it adds debug code for allocated but
not freed memory

http://vividos.home.pages.de - vividos@asamnet.de
(c) 1999 Michael Fink
*/

#include "hd60.h"

//#define DEBUG_IT

#ifdef DEBUG_IT
 #include <alloc.h>
 #include <stdio.h>
 static ulong debug_it_size=0;
 void deb_start(){ debug_it_size=farcoreleft(); };
 void deb_end(){
   ulong delta=debug_it_size-farcoreleft();
   if (delta!=0) printf("%lu bytes were not 'delete'd\n",delta);
 };
 #pragma startup deb_start 64
 #pragma exit deb_end 64
#endif

int main(char argc,char**argv){
  hotdir *hd=new hotdir(argv[0]);

  for(int n=1;n<argc;n++)
    hd->argument(argv[n]);

  hd->start();

  delete hd;

  return 0;
};

