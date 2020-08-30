// feldlini.h - magnetische feldlinien
// (c) 1997 Michael Fink
// https://github.com/vividos/OldStuff/

#include "defs.h"
#include "vektor.h"      // stellt Vektor-Klasse bereit
#include "linmap.h"

class graphic;

class feldlinien
{
// nach au·en geschÅtzte Daten
private:
  float delta;

  // Festlegung struct punktladung
  struct punktladung
  {
    vektor p; // Koordinaten der Punktladung
    float q;  // Ladung
  };

  // Array aus punktladung
  punktladung* pl;

  // Anzahl an Ladungen im Array
  int ladungen;

  // Objekt, das Grafikfunktionen bereitstellt
  graphic* gr;

  // Objekte, die vom virtuellen ins Grafik-
  // Koordinatensystem umrechnen
  linear_mapping cx, cy;

public:
  // Konstruktor
  feldlinien();

  // Destruktor
  ~feldlinien();

  // um Ladungen einzufÅgen
  void set_ladung(float x, float y, float q);

  // beginnt zu zeichnen
  void start();

private:
  // zeichnet Punkt im virtuellen Koordinatensystem
  void setpix(float x, float y, byte color);

  // Kreis bein x,y mit Radius rad zeichnen
  void kreis(float x, float y, float rad, byte color);

  void verfolge_feldlinie(float winkel, int von);

  // berechnet die Richtung des FeldstÑrkevektors bei 'wo' von einer
  // Ladung; plus gibt an, welches Vorzeichen die Ladung hat, von
  // der die Feldlinie ausgeht
  vektor berechne_feldst(vektor wo, int von_ladung, bool plus);
};
