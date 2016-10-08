/*
   d64view - a d64 disk image viewer
   Copyright (c) 2002,2003 Michael Fink

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

   $Id: canvas.cpp,v 1.6 2003/09/08 20:55:45 vividos Exp $

*/
/*! \file canvas.cpp

   \brief canvas implementation

   canvas implementation; drawing is done using the charset class that
   contains a bitmap of all viewable chars.

*/

// needed includes
#include "common.hpp"
#include "canvas.hpp"
#include "app.hpp"


// event table


BEGIN_EVENT_TABLE(c64screen_canvas, wxScrolledWindow)
   EVT_ERASE_BACKGROUND(c64screen_canvas::OnEraseBackground)
END_EVENT_TABLE()


// c64screen_canvas methods

c64screen_canvas::c64screen_canvas(unsigned int xsize, unsigned int ysize,
   wxWindow* parent, const wxPoint& pos, long style)
:wxScrolledWindow(parent, -1, pos, wxSize(xsize*16+16,ysize*16+16), style),
 xres(xsize), yres(ysize), charset(wxGetApp().get_charset())
{
   screen.resize(xres*yres,0x20);
   colored.resize(xres*yres,false);

   // add scroll bars
   SetScrollbars(16,16, xres+1,yres+1);

   invert = false;
   uppercase = false;
}

void c64screen_canvas::print(unsigned int xpos, unsigned int ypos,
   const char* text)
{
   std::string thetext(text);

   charset.remap_ascii_string(thetext);

   unsigned int offset = invert? 128 : 0;

   // store text in screen array
   unsigned int max = thetext.size();
   for(unsigned int i=0; i<max; i++)
      screen[ypos*xres + xpos + i] = thetext[i]^offset;
}

void c64screen_canvas::OnDraw(wxDC& dc)
{
   dc.BeginDrawing();

   // background brush
   wxBrush bg_brush;
   bg_brush.SetColour(color_dark_blue);

   dc.SetBackground(bg_brush);
   dc.Clear();
   dc.SetBackground(wxNullBrush);

   // red brush for marked chars
   wxBrush red_brush;
   red_brush.SetColour(color_red);
   dc.SetBrush(red_brush);

   // draw screen
   wxMemoryDC memdc;
   memdc.SelectObject(charset.get_bitmap());

   unsigned int yoffset = uppercase? 256 : 0;

   for(unsigned int y=0; y<yres; y++)
   for(unsigned int x=0; x<xres; x++)
   {
      // draw each character
      c64_byte ch = screen[y*xres + x];

      if (colored[y*xres+x])
         dc.DrawRectangle(wxPoint(x*16+8,y*16+8),wxSize(16,16));

      dc.Blit(x*16+8,y*16+8, 16,16, &memdc, (ch&0x0f)<<4, (ch&0xf0)|yoffset, wxOR);
   }

   dc.SetBrush(wxNullBrush);
   memdc.SelectObject(wxNullBitmap);

   dc.EndDrawing();
}

void c64screen_canvas::OnEraseBackground(wxEraseEvent& event)
{
   wxDC* dc = event.GetDC();

   if (dc!=NULL)
   {
      // background brush
      wxBrush bg_brush;
      bg_brush.SetColour( color_dark_blue );

      dc->SetBackground(bg_brush);
      dc->Clear();
      dc->SetBackground(wxNullBrush);
   }
}
