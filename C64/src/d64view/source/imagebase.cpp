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

   $Id: imagebase.cpp,v 1.3 2003/09/08 20:55:46 vividos Exp $

*/
/*! \file imagebase.cpp

   \brief image base class methods

   general disk image base functions

*/

// needed includes
#include "common.hpp"
#include "imagebase.hpp"


// disk_image_base methods

unsigned int disk_image_base::map_to_blocks(unsigned int track,unsigned int sector)
{
   unsigned int numblock = 0;

   for(unsigned int n=0; n<track; n++)
      numblock += sectors_per_track[n];

   numblock += sector;

   return numblock;
}
