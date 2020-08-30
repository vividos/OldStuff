// antchaos.cpp - verbesserte antchaos-version
// (c) 1999 Michael Fink

/*
Originaler Kommentar von Christian Strobl:
Programm, das mittels eines Graphikbeispiels verdeutlicht, wie
in vîllig zufÑlligen Strukturen, also Chaos, plîtzlich regel-
mÑ·ige Muster auftauchen kînnen, also Ordnung.
Das Programm fÅllt zuerst per Zufallsgenerator den Bildschirm
mit einer gleichen Anzahl von roten und gelben Punkten. Die
roten Punkte stellen Ameisen dar und die gelben Nahrung. An-
schlie·end werden die Ameisen per Zufallsgenerator auf dem
Bildschirm hin- und herbewegt. Stî·t eine Ameise auf Nahrung,
so hebt sie sie auf und nimmt sie mit, bis sie wieder auf
Nahrung trifft. Dort lÑ·t sie das alte fallen und hebt das neue
auf.
Nach einer gewissen Zeit, zeigen sich plîtzlich regelmÑ·ige
Muster in der Verteilung der Nahrung auf dem Bildschirm, so
kann sich zum Beispiel die gesamte Nahrung in der Mitte des
Bildschirms ansammeln o.Ñ.
Somit ist also aus vîlligem Chaos ( zufÑllige Verteilung von
Ameisen und Nahrung, zufÑllige Bewegungen der Ameisen ),
schlie·lich Ordnung hervorgegeangen.
*/

#define AMEISENANZAHL 2000
#define NAHRUNGSANZAHL 2000
#define ROT 4
#define GELB 14
#define GRUEN 2
#define BLAU 1
#define SCHWARZ 0

#define XMAX 320
#define YMAX 200

// -------------------------------

#include "defs.h"
#include "xmode.h"

#include <stdlib.h>
#include <conio.h>

class ant
{
  xmode* xm;
  word ameisen[AMEISENANZAHL][3];
  word nahrung[NAHRUNGSANZAHL][2];
public:
  ant();

  void start();

  void male_schirm();
  void bewege_ameisen();

private:
  void arrays_fuellen();
};

ant::ant()
{
  xm = new xmode;

  randomize();

  arrays_fuellen();
};

void ant::start()
{
  xm->on();
  xm->clearpage(1);
  xm->clearpage(0);
  xm->setpage(0);
  xm->showpage(1);

  male_schirm();
  xm->switchpage();

  while (!kbhit())
  {
    // copy pixels from currently set page to currently active page
    xm->pagemove(1 - xm->activepage());

    bewege_ameisen();

    xm->switchpage();
  };

  if (getch() == 0)
    getch();

  if (getch() == 0)
    getch();

  xm->off();
};

void ant::arrays_fuellen()
{
  // Ameisen zufÑllig verteilen
  for (int a = 0; a < AMEISENANZAHL; a++)
  {
    ameisen[a][0] = random(XMAX);
    ameisen[a][1] = random(YMAX);
    ameisen[a][2] = ROT;
  };

  // Brot zufÑllig verteilen
  for (int n = 0; n < NAHRUNGSANZAHL; n++)
  {
    nahrung[n][0] = random(XMAX);
    nahrung[n][1] = random(YMAX);
  };
};

void ant::male_schirm()
{
  for (int a = 0; a < AMEISENANZAHL; a++)
    xm->setpix(ameisen[a][0], ameisen[a][1],
       ameisen[a][2]);

  for (int n = 0; n < NAHRUNGSANZAHL; n++)
    xm->setpix(nahrung[n][0], nahrung[n][1], GELB);
};

void ant::bewege_ameisen()
{
  for (int a = 0; a < AMEISENANZAHL; a++)
  {
    byte richtung = random(8); // 8 mîgliche Richtungen

    int neux = ameisen[a][0];
    int neuy = ameisen[a][1];

//                                        y
//                              0 1 2     A
// Nummerierung der Richtungen: 3 X 4     |
//                              5 6 7     +--> x

    switch (richtung)
    {
      case 0: neux--; neuy--; break;
      case 1:         neuy--; break;
      case 2: neux++; neuy--; break;
      case 3: neux--;         break;
      case 4: neux++;         break;
      case 5: neux--; neuy++; break;
      case 6:         neuy++; break;
      case 7: neux++; neuy++; break;
    };

    byte neufarbe = xm->getpix(neux, neuy);

    // neue Position nicht innerhalb des Bildschirms
    // oder Farbe des neuen Feldes nicht unbesetzt oder Nahrung
    if (neux < 0 || neux >= XMAX || neuy < 0 || neuy >= YMAX ||
        (neufarbe != SCHWARZ && neufarbe != GELB))
      continue;

    word altfarbe = ameisen[a][2];

    // altes Objekt lîschen bzw. alte Nahrung ablegen
    xm->setpix(ameisen[a][0], ameisen[a][1],
      altfarbe == BLAU ? GELB : SCHWARZ);

    switch (altfarbe)
    {
      case ROT: // Ameise ohne Nahrung
        if (neufarbe == SCHWARZ) // neues Feld unbesetzt
          neufarbe = ROT;
        else // neues Feld mit Nahrung besetzt
          neufarbe = GRUEN;
        break;

     default: // Ameise vorher grÅn oder blau, mit Nahrung
        if (neufarbe == SCHWARZ) // neues Feld unbesetzt
        {
          xm->setpix(ameisen[a][0], ameisen[a][1], SCHWARZ);

          // neue Ameise wieder mit Last (grÅn) malen
          neufarbe = GRUEN;
        }
        else // neues Feld mit Nahrung besetzt
        {
          xm->setpix(ameisen[a][0], ameisen[a][1], GELB);

          // neue Ameise beim Umladen (blau) malen
          neufarbe = BLAU;
        }
        break;
    };

    // neue Ameise setzen
    ameisen[a][0] = neux;
    ameisen[a][1] = neuy;
    ameisen[a][2] = neufarbe;

    xm->setpix(neux, neuy, neufarbe);
  };
};

void main()
{
  ant a;
  a.start();
};
