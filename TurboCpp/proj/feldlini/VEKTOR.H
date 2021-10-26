// vektor.h - 2D Vektor
// (c) 1997 Michael Fink

#ifndef __vektor_h_
#define __vektor_h_

#include <math.h>        // fÅr sin, cos, fabs usw.

// Kreiszahl
const float pi = 3.1415927;

// Klasse stellt einen 2D Vektor dar
class vektor
{
public:
  // Elemente des Vektors
  float x, y;

  vektor(){ x = 0.0; y = 0.0; }
  vektor(double x1,double y1){ x = x1; y = y1;}

  // Rechenoperationen
  void operator +=(vektor v) { x += v.x; y += v.y; };
  void operator -=(vektor v) { x -= v.x; y -= v.y; };
  void operator *=(float f) { x *= f; y *= f; };

  // liefert LÑnge des Vektors
  float laenge() { return sqrt(x * x + y * y); };

  void einheitsvektor() { float l = laenge(); x /= l; y /= l; };

  // macht Vektor aus Ortsvektoren
  void vektor_von_zu(vektor v, vektor w) { x = w.x - v.x; y = w.y - v.y; };

  // macht Vektor aus gegebener LÑnge und Winkel
  void vektor_winkel_laenge(float winkel, float laenge)
  {
    x = laenge * cos((winkel / 360) * 2 * pi);
    y = laenge * sin((winkel / 360) * 2 * pi);
  };
};

#endif
