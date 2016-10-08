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

   $Id: about.hpp,v 1.1 2003/09/08 20:56:49 vividos Exp $

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
