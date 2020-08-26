// fractal.cpp - Mandelbrot fractal generator
// (c) 1999 Michael Fink
// https://github.com/vividos/OldStuff/

#include "defs.h"
#include "xmode.h"
#include "linmap.h"

#include <conio.h>

class fractal
{
  xmode* xm;
  linear_mapping xmap;
  linear_mapping ymap;
  int maxiter;
public:
  fractal();
  ~fractal();

  void setXrange(float xmin, float xmax);
  void setYrange(float ymin, float ymax);
  void setMaxIter(int max){ maxiter = max; };

  void start();

private:
  int iterate(float x, float y);
};

fractal::fractal()
{
  xm = new xmode;
};

fractal::~fractal()
{
  delete xm;
};

void fractal::setXrange(float xmin,float xmax)
{
  xmap = linear_mapping(xmin, xmax, 0, 320);
};

void fractal::setYrange(float ymin,float ymax)
{
  ymap = linear_mapping(ymin, ymax, 0, 200);
};

void fractal::start()
{
  xm->on();

  byte linebuf[320];

  for (int y = 0; y < 320; y++)
  {
    for(int x = 0; x < 320; x++)
      linebuf[x] = (byte)iterate(
        xmap.long2float(x), ymap.long2float(y));

    xm->setline(0, y, linebuf, 320);

    if (kbhit())
    {
      if (getch()==0)
        getch();
      break;
    };

  };

  if (getch()==0)
    getch();

  xm->off();
};

int fractal::iterate(float x, float y)
{
  int iter = 0;
  float re = 0.0;
  float im = 0.0;

  float help;

  do
  {
    help = re;
    re = re * re - im * im + x;
    im = 2 * help * im + y;

    iter++;
  }
  while (iter < maxiter && (re * re + im * im) < 4);

  return iter;
};

void main()
{
  fractal f;

  f.setMaxIter(255);
  f.setXrange(-8./3., 8./3.);
  f.setYrange(-2., 2.0);

  f.start();
};
