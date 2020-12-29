// grdemo.cpp - graphic demo program
// (c) 1999, 2020 Michael Fink
// https://github.com/vividos/OldStuff/

#include "graphic.h"
#include <stdio.h>
#include <conio.h>

void draw_line(graphics& gr,
  word fromx, word fromy,
  word tox, word toy)
{
  byte pixel = 1;
  float slope = float(toy - fromy) / float(tox - fromx);
  float offset = fromy;
  for (word curx = fromx; curx < tox; curx++)
  {
    word y = (word)( (slope * curx) + offset );
    gr.setpix(curx, y, &pixel);
  }
}

void main()
{
  printf("Graphics library demo app - tries out various graphics modes\n\n");

  const word resolutions[8] =
  {
    320, 200,
    640, 480,
    800, 600,
    1024, 768
  };

  for (unsigned int resIndex = 0; resIndex < 8; resIndex += 2)
  {
    word resx = resolutions[resIndex];
    word resy = resolutions[resIndex + 1];

    graphics* gr = graphics::find_mode(
      resx, resy,
      8, // bits per pixel
      true);

    if (gr == NULL)
    {
      printf("Couldn't find video mode for resolution %ix%i",
        resx, resy);
      continue;
    }

    gr->on();

    // draw an X across the screen
    draw_line(*gr, 0, 0, resx, resy);
    draw_line(*gr, 0, resy, resx, 0);

    getch();
    gr->off();
  }
};
