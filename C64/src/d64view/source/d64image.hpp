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

   $Id: d64image.hpp,v 1.4 2003/09/08 20:55:45 vividos Exp $

*/
/*! \file d64image.hpp

   \brief d64 image class

   handles d64 image loading

*/

// import guard
#ifndef d64view_d64image_hpp_
#define d64view_d64image_hpp_

// needed includes
#include "imagebase.hpp"


// classes

//! d64 image class
class d64_image: public disk_image_base
{
public:
   d64_image(){}
   virtual ~d64_image(){}

   //! loads disk image
   virtual bool load_image(const char* filename);
};


#endif // d64view_d64image_hpp_
