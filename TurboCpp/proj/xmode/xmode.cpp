// xmode.cpp - a wrapper class for the 320x200 vga xmode
// (c) 1999 Michael Fink
// https://github.com/vividos/OldStuff/
// see 'xmode.h' for details

#include "xmode.h"

// segment address of currently active page
extern word vio_seg;

extern "C"
{
  void init320200();
  void exitvideo();
  void wait_retrace();
  void showpage(byte page);
  void setpage(byte page);
  void switchpage();
  void setpix(int x, int y, byte farbe);
  byte getpix(int x, int y);
  byte far* getfontptr();

  void clearpage(int page);

  // kopiert 'page' nach aktuell bearbeiteter page
  void pagemove(int page);

  void line_copy(int plane, byte* linebuffer, word offset, word bufsize);

  void blockmove(int vonp, int vonx, int vony,
    int nachp, int nachx, int nachy, int breite, int hoehe);
};


xmode::xmode()
{
  mode_on = false;
};

xmode::~xmode()
{
  if (mode_on)
    off();
};

void xmode::on()
{
  ::init320200();
  mode_on = true;
  setpage(1);
  showpage(0);
};

void xmode::off()
{
  ::exitvideo();
  mode_on = false;
};

void xmode::setpix(word x, word y, byte farbe)
{
  ::setpix(x, y, farbe);
};

byte xmode::getpix(word x, word y)
{
  return ::getpix(x, y);
};

void xmode::setline(word x, word y, byte* buffer, word length)
{
  ::line_copy(x & 3, buffer, (x >> 2) + y * 80, length);
};

byte xmode::activepage()
{
  return (vio_seg - 0xa000) >> 10;
};

void xmode::showpage(byte page)
{
  ::showpage(page);
};

void xmode::setpage(byte page)
{
  ::setpage(page);
};

void xmode::switchpage()
{
  ::switchpage();
};

void xmode::clearpage(byte page)
{
  ::clearpage(page);
};

void xmode::pagemove(byte page)
{
  ::pagemove(page);
};

void xmode::printchar(char ch, word x, word y, byte vg, byte hg)
{
  typedef byte ZDEF[256][8];                /* Aufbau des Zeichensatz */
  typedef ZDEF far* ZPTR;                   /* Zeiger auf Zeichensatz */

  byte i, k,                                       /* SchleifenzÑhler */
       Maske;                      /* Bitmaske fÅr Aufbau des Zeichens */

  static ZPTR fptr = (ZPTR)0;        /* Zeiger auf Zeichensatz im ROM */

  if (fptr == (ZPTR)0)     /* Zeiger auf Zeichensatz schon ermittelt? */
    fptr = (ZPTR)getfontptr();  /*Nein, mit Hilfe der Assembler-Funktion laden*/

  /*- das Zeichen Punkt fÅr Punkt aufbauen ----------------------------*/

  if (hg == 255)             /* Zeichen transparent zeichnen? */
    for (i = 0; i < 8; ++i)      /* Ja, nur Vordergrund-Punkte setzen */
    {
      Maske = (*fptr)[ch][i];       /* Bitmuster fÅr eine Zeile holen */
      for (k = 0; k < 8; ++k, Maske <<= 1) /* die Spalten durchlaufen */
        if (Maske & 128)                             /* Punkt setzen? */
          setpix(x + k, y + i, vg);                             /* Ja */
    }
  else                                    /* Nein, jeden Punkt setzen */
    for (i = 0; i < 8; ++i)                     /* Zeilen durchlaufen */
    {
      Maske = (*fptr)[ch][i];       /* Bitmuster fÅr eine Zeile holen */
      for (k = 0; k < 8; ++k, Maske <<= 1) /* die Spalten durchlaufen */
        setpix(x + k, y + i, (byte)((Maske & 128) ? vg : hg));
    }
}

void xmode::print(word x, word y, byte foreground, byte background, const char* text)
{
  for (const char* cp = text; *cp; ++cp, x += 8)
    printchar(*cp, x, y, foreground, background);
}
