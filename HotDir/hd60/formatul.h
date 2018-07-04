/* formatul.h - formats an unsigned long to a char string

the format_ulong() function transforms an unsigned long to a string,
inserting points at every 10^3

http://vividos.home.pages.de - vividos@asamnet.de
(c) 1999 Michael Fink
*/

#ifndef __formatul_h_
#define __formatul_h_

#include "defs.h"

char *format_ulong(ulong u,char*buffer);

#endif

