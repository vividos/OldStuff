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

   $Id: charset.hpp,v 1.4 2003/05/19 21:08:14 vividos Exp $

*/
/*! \file charset.hpp

   \brief c64 character set handling

   class to manage c64 character set loading and drawing

*/

// include guard
#ifndef d64view_charset_hpp_
#define d64view_charset_hpp_

// needed includes


// classes

//! charset class
class c64_charset
{
public:
   //! ctor
   c64_charset(){}

   //! initializes charset
   void init();

   //! remaps ascii string to c64 screen codes
   void remap_ascii_string(std::string& str);

   //! returns charset bitmap
   wxBitmap& get_bitmap(){ return charset_bmp; }

protected:
   //! bitmap with all chars on it
   wxBitmap charset_bmp;
};


#endif // d64view_charset_hpp_
