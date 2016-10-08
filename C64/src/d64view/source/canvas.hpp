/*
   d64view - a d64 disk image viewer
   Copyright (c) 2002,2003,2016 Michael Fink
*/
/*! \file canvas.hpp

   \brief canvas window class

   canvas window class that looks like a c64 screen; it can be scrolled when
   needed and text can be filled with print().

*/

// import guard
#ifndef d64view_canvas_hpp_
#define d64view_canvas_hpp_

// needed includes
#include "charset.hpp"


// classes

//! c64 screen canvas
class c64screen_canvas: public wxScrolledWindow
{
public:
   //! ctor
   c64screen_canvas(unsigned int xsize, unsigned int ysize,
      wxWindow* parent, const wxPoint& pos, long style);

   //! sets used charset, either "uppercase/special chars" or "lowercase/uppercase"
   void set_charset(bool upper){ uppercase = upper; }

   //! sets inverted mode; affects next print()
   void set_invert(bool inv){ invert = inv; }

   //! sets red background color for a specific character
   void set_color(unsigned int xpos, unsigned int ypos, bool set)
   {
      colored[ypos*xres+xpos] = set;
   }

   //! prints a text on the screen
   void print(unsigned int xpos, unsigned int ypos,
      const char* text);

   //! draws canvas
   virtual void OnDraw(wxDC& dc);

   //! erases background
   void OnEraseBackground(wxEraseEvent& event);

protected:
   //! resolution of screen
   unsigned int xres, yres;

   //! use uppercase charset?
   bool uppercase;

   //! invert chars?
   bool invert;

   //! screen bytes
   std::vector<c64_byte> screen;

   //! screen background change
   std::vector<bool> colored;

   //! charset to use
   c64_charset& charset;

   //! device context with screen bitmap
   wxMemoryDC screen_dc;

private:
   DECLARE_EVENT_TABLE()
};


#endif
