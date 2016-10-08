/*
   d64view - a d64 disk image viewer
   Copyright (c) 2002,2003,2016 Michael Fink
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
