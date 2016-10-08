/*
   d64view - a d64 disk image viewer
   Copyright (c) 2002,2003,2016 Michael Fink
*/
/*! \file about.hpp

   \brief about dialog

   about dialog shown in the "Help -> About" dialog.

*/

// include guard
#ifndef d64view_about_hpp_
#define d64view_about_hpp_

// needed includes
#include "canvas.hpp"
#include "version.hpp"
#include <algorithm>


// constants

//! c64 canvas sizes
const unsigned int about_width = 24;
const unsigned int about_height = 6;


// classes

//! about dialog class
class d64view_about_dlg: public wxDialog
{
public:
   //! ctor
   d64view_about_dlg(wxWindow* parent)
      : wxDialog(parent, -1, wxString("About"), wxDefaultPosition,
         wxSize((about_width+2)*16+8,(about_height+5)*16+8))
   {
      // OK button
      ok_button = new wxButton(this, wxID_OK, "&OK", wxPoint(8, (about_height+2)*16));

      wxLayoutConstraints* layout = new wxLayoutConstraints();

      layout->width.AsIs();
      layout->height.AsIs();
      layout->top.AsIs();
      layout->centreX.SameAs(this, wxCentreX);

      ok_button->SetConstraints(layout);
      Layout();

      // c64 canvas with copyright infos
      canvas = new c64screen_canvas(about_width,about_height, this, wxPoint(8,8), 0);

      canvas->set_charset(true);

      std::string ver(d64view_version);
      std::transform(ver.begin(),ver.end(),ver.begin(),toupper);

      canvas->print(0,0,ver.c_str());
      canvas->print(0,2,"cOPYRIGHT (c) 2002,2003");
      canvas->print(0,3," mICHAEL fINK");
      canvas->print(0,5,"HTTP://WWW.VIVIDOS.DE/");
   }

protected:
   //! ok button
   wxButton* ok_button;

   // c64 canvas for copyright infos
   c64screen_canvas* canvas;
};


#endif
