/*
   d64view - a d64 disk image viewer
   Copyright (c) 2002,2003,2016 Michael Fink
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
