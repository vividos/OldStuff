/*
   d64view - a d64 disk image viewer
   Copyright (c) 2002,2003,2016 Michael Fink
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
