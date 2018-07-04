/* formatul.cpp - formats an unsigned long to a char string

see 'formatul.h' for details

http://vividos.home.pages.de - vividos@asamnet.de
(c) 1999 Michael Fink
*/

#include "formatul.h"

#include <stdlib.h>
#include <string.h>

// formatiert einen ulong mit Tausenderpunkten in einen char-String
char *format_ulong(ulong u,char*buffer){
  ultoa(u,buffer,10); // wandelt erstmal um

  word len=(word)strlen(buffer); // Laenge rausbekommen
  if (len>3){ // wenn Laenge > 3
    for(int i=len-3;i>0;i-=3){ // alle 3 chars durchgehen
      len=(word)strlen(buffer); // *aktuelle* Laenge holen
      for(int j=len;j>=i;j--) // Platz fuer Punkt schaffen
	buffer[j+1]=buffer[j];
      buffer[i]='.'; // und Punkt setzen
    };
  };
  return buffer; // fuer Funktionen, die einen String erwarten
};
