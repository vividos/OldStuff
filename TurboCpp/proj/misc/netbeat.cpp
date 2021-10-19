// netbeat.cpp - Netbeat timer display
// https://github.com/vividos/OldStuff/
// (c) 1999, 2021 Michael Fink

#include <dos.h>
#include <stdio.h>
#include <conio.h>

void main()
{
  printf("Netbeat:\n");
  time t;
  unsigned long sekunden, netbeat;
  char lastdigit = 10;

  do
  {
    do
    {
      gettime(&t);

      sekunden = t.ti_hour * 3600L + t.ti_min * 60L + t.ti_sec;
      sekunden *= 100L;
      sekunden += t.ti_hund;
      sekunden *= 10L;
      netbeat = (sekunden / 864);
    }
    while ((netbeat & 7) == lastdigit);

    lastdigit = netbeat & 7;

    printf("@%03u://%02u\r", (unsigned)((netbeat) / 100), ((unsigned)netbeat % 100));
  } while (kbhit() == 0);

  if (getch() == 0) getch();
};
