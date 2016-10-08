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

   $Id: charset.cpp,v 1.9 2003/09/08 20:55:45 vividos Exp $

*/
/*! \file charset.cpp

   \brief charset handling implementation

   charset loading and ascii string remapping

*/

// needed includes
#include "common.hpp"
#include "charset.hpp"
#include <algorithm>


// externals

extern c64_byte chargen[4096];


// c64_charset methods

/*! initializes c64 charset by creating a wxBitmap with all c64 characters in
    it, loaded from the "chargen" array (from c64 rom at $d000-$dfff) */
void c64_charset::init()
{
   c64_byte* buffer = chargen;

   // prepare bitmap
   charset_bmp.Create(256,512);

   wxMemoryDC memdc;
   memdc.SelectObject(charset_bmp);

   // background pen
   memdc.SetBackground(*wxTRANSPARENT_BRUSH);
   memdc.Clear();

   // drawing pen
   wxPen draw_pen;
   draw_pen.SetColour( color_light_blue );

   memdc.SetPen(draw_pen);

   // y goes from 0 to 32 cause there are 2 charsets, one with capitals only, and
   // one with uppercase and lowercase letters which starts at y = 16;
   for(int y=0;y<32;y++)
   for(int x=0;x<16;x++)
   {
      // calculates the position in the charset data array
      int charsetpos = int( ( (x&15) + ( (y&15) << 4 )) & 0xff ) << 3;

      // when y >= 16, add 2048 to the offset
      charsetpos += (y & 16) << (11-4);

      // paint-loop for one particular char
      for(int dy=0;dy<8;dy++)
      {
         // gets the current byte which holds a pixel line with 8 pixels
         unsigned char charsetbyte = buffer[ charsetpos + dy ];
         
         for(int dx=0;dx<8;dx++)
         {
            // is the topmost bit set?
            bool bit = (charsetbyte & 0x80) == 0x80;

            // when yes, paint pixels
            if (bit)
            {
               int cx = (x << 4) + (dx << 1);
               int cy = (y << 4) + (dy << 1);

               memdc.DrawPoint(cx,cy);
               memdc.DrawPoint(cx+1,cy);

               // to have a non-interlaced charset, set pixels at
               // (cx,cy+1) and (cx+1,cy+1), too
            }

            // shifts to the next bit
            charsetbyte <<= 1;
         }
      }
   }

   // free memory dc resources
   memdc.SetPen(wxNullPen);

   memdc.SelectObject(wxNullBitmap);
}


//! remaps ascii characters to c64 string codes
/*! the function does the following mapping:
<pre>
   screen code       ascii code        further codes     part in code
   0x00 - 0x20       0x40 - 0x60                         (1)
   0x20 - 0x40       0x20 - 0x40                         n/a (no remap)
   0x40 - 0x60       0x60 - 0x80                         (3)
   0x60 - 0x80       0xA0 - 0xC0                         (4)

   0x40 - 0x60       0x60 - 0x80       0xC0 - 0xE0       (5)
   0x60 - 0x7E       0xA0 - 0xBE       0xE0 - 0xFE       (6)
   0x5E              0x7E              0xFF              (7)

   n/a               0x00 - 0x20
   n/a               0x80 - 0xa0
</pre>
*/
char remap_ascii_string(unsigned char c)
{
   // remap
   if ( 0xc0 <= c && c < 0xe0 ) c -= 0x60;      // (5)
   if ( 0xe0 <= c && c < 0xff ) c -= 0x20;      // (6)
   if ( c == 0xff ) c = 0x7e;                   // (7)

   if ( 0x40 <= c && c < 0x60 ) c -= 0x40; else // (1)
   if ( 0x60 <= c && c < 0x80 ) c -= 0x20; else // (3)
   if ( 0xa0 <= c && c < 0xc0 ) c -= 0x40;      // (4)

   return c;
}

/*! remaps ascii strings to c64 screen codes using remap_ascii_string() */
void c64_charset::remap_ascii_string(std::string& str)
{
   std::transform(str.begin(),str.end(),str.begin(),::remap_ascii_string);
}
