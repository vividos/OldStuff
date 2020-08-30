// feldlini.cpp - magnetische feldlinien
// (c) 1997 Michael Fink
// https://github.com/vividos/OldStuff/

#include <conio.h>       // fÅr Farben
#include "feldlini.h"
#include "graphic.h"
#include "keyboard.h"

const int max_ladungen = 16;
const float e0 = 8.854e-12;          // elektrische Feldkonstante
const float e = 1.602e-19;           // Elementarladung in C
const int d_winkel = 20;             // delta-Winkel, siehe unten
const int max_punkte = 300;          // maximal zu zeichnende Punkte

feldlinien::feldlinien()
{
  // Speicher fÅr Array pl
  pl = new punktladung[max_ladungen];

  gr = new graphic();

  // linear_mapping-Objekte initialisieren
  // -4 < x < 4  und  -3 < y < 3
  cx = linear_mapping(-4, 4, 0, gr->getxres() - 1);
  cy = linear_mapping(-3, 3, 0, gr->getyres());

  // Abstand von einem Punkt zum nÑchsten
  delta = fabs(cx.long2float(0) - cx.long2float(1));

  // noch keine Ladungen
  ladungen = 0;
};

feldlinien::~feldlinien()
{
  // lîscht alle Objekte
  delete pl;
  delete gr;
}

void feldlinien::set_ladung(float x, float y, float q)
{
  // schon zuviele Ladungen?
  if (ladungen >= max_ladungen)
    return;

  // ins Array einfÅgen und ZÑhler erhîhen
  pl[ladungen].p.x = x;
  pl[ladungen].p.y = y;
  pl[ladungen].q = q;

  ladungen++;
};

void feldlinien::start()
{
  // Grafikmode anschalten
  gr->on();

  // Feldlinien der Ladungen verfolgen
  int i;
  for (i = 0; i < ladungen; i++)
    for (int w = 0.5 * d_winkel; w < 360; w += d_winkel)
      verfolge_feldlinie(w, i);

  // Kreise um Punktladungen zeichnen
  for (i = 0; i < ladungen; i++)
  {
    // Farbe je nach Vorzeichen der Ladung
    byte color = (pl[i].q > 0) ? RED : BLUE;

    // Kreis mit Radien von 5 bis 10 * delta
    for (int j = 5; j < 10; j++)
    {
      kreis(pl[i].p.x, pl[i].p.y, delta * j, color);
    }
  };

  // auf Tastendruck warten
  keyboard::wait_key();

  // Grafik ausschalten
  gr->off();
};

void feldlinien::setpix(float x, float y, byte color)
{
  long x1 = cx.float2long(x);
  long y1 = cy.float2long(-y);

  // Grenzen ÅberprÅfen
  if ((x1 < 0) || (x1 >= gr->getxres()) ||
    (y1 < 0) || (y1 >= gr->getyres()))
    return;

  // Punkt setzen
  gr->setpix((int)x1, (int)y1, color);
};

void feldlinien::kreis(float x, float y, float rad, byte color)
{
  // Kreis bei (x, y) mit Radius rad und Farbe color zeichnen
  int x1 = (int)cx.float2long(x);
  int y1 = (int)cy.float2long(-y);
  int r = (int)cx.float2long(x + rad) - x1;

  // Kreisabschnitte von 0¯ bis 45¯
  int max = r * 0.71;
  for (int i = 0; i < max; i++)
  {
    // x˝ + y˝ = r˝ umgeformt
    int s = sqrt(r * r - i * i);

    // Kreisabschnitte zeichnen
    gr->setpix(x1 + i, y1 + s, color);
    gr->setpix(x1 + i, y1 - s, color);
    gr->setpix(x1 - i, y1 + s, color);
    gr->setpix(x1 - i, y1 - s, color);

    gr->setpix(x1 + s, y1 + i, color);
    gr->setpix(x1 + s, y1 - i, color);
    gr->setpix(x1 - s, y1 + i, color);
    gr->setpix(x1 - s, y1 - i, color);
  };
};

void feldlinien::verfolge_feldlinie(float winkel, int von)
{
  // verfolgt Feldlinien, die sternfîrmig von der Punktladung
  // Nr. 'von' im Winkel 'winkel' ausgehen

  // Probeladungsposition noch bei Punktladung
  vektor jetzt = pl[von].p;

  // ist Punktladung positiv?
  bool ist_pl_plus = pl[von].q > 0 ? true : false;

  // Vektor gibt die nÑchste Richtung der Probeladung an
  vektor richtung;
  // Vektor ist 2*delta lang
  richtung.vektor_winkel_laenge(winkel, 2 * delta);

  // zu jetzt addieren
  jetzt += richtung;
  // Punkt malen
  setpix(jetzt.x, jetzt.y, YELLOW);

  // Abbruchskriterium, noch nicht erfÅllt
  bool abbruch = false;

  // zÑhlt Anzahl der gesetzen Punkte
  int zaehler = 0;

  // Hauptschleife
  do
  {
    // Summenvektor der FeldstÑrkevektoren
    vektor summe = { 0, 0 };

    // alle Vektoren zusammenzÑhlen
    // bei Probeladungspunkten
    for (int z = 0; z < ladungen; z++)
      summe += berechne_feldst(jetzt, z, ist_pl_plus);

    // Vektor 'delta' lang machen
    summe.einheitsvektor();
    summe *= delta;
    jetzt += summe;

    // Punkt malen
    setpix(jetzt.x, jetzt.y, YELLOW);

    // mitzÑhlen
    zaehler++;

    // grî·er als max_punkte?
    if (zaehler > max_punkte)
      abbruch = true;

    // Abstand zu Punktladung
    vektor abstand;
    // alle ÅberprÅfen
    for (z = 0; z < ladungen; z++)
    {
      abstand.vektor_von_zu(jetzt, pl[z].p);
      if (abstand.laenge() < 2 * delta)
        abbruch = true;    // Abbruch, wenn Abstand < 2*delta
    };
  } while (!abbruch);  // solange, bis Abbruchskriterium erfÅllt
};

vektor feldlinien::berechne_feldst(vektor wo, int von_ladung,
  bool plus)
{
  // Vektor der FeldstÑrke
  vektor feldst;
  // Vektor von 'wo' zur Punktladung
  feldst.vektor_von_zu(wo, pl[von_ladung].p);

  // wenn Ladung und Punktladung gleiches Vorzeichen
  // haben, Vektor von der Punkt-
  // ladung zu 'wo' gehen lassen
  if ((pl[von_ladung].q > 0 && plus) ||
      (pl[von_ladung].q < 0 && !plus))
    feldst *= -1;

  // Abstand der Punkte als r retten
  float r = feldst.laenge();
  // eine Einheit lang machen
  feldst.einheitsvektor();

  // FeldstÑrke berechnen
  float f = (pl[von_ladung].q) / (4 * pi * e0 * r * r);

  // Kraftvektor mit 'FeldstÑrke-LÑnge'
  feldst *= fabs(f);

  return feldst;
};

// Hauptfunktion
void main()
{
  // Objekt Feldlinien
  feldlinien fl;

  // Ladungen eintragen
  fl.set_ladung(-1, 0, -e);
  fl.set_ladung(1, 0, e);

  // und zeichnen
  fl.start();
}
