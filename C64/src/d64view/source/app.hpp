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

   $Id: app.hpp,v 1.10 2003/09/08 20:55:45 vividos Exp $

*/
/*! \file app.hpp

   \brief application class

   the wxApp derived class that runs the application. There are some publicly
   available functions to access some functionality used throughout the
   application.

*/

// include guard
#ifndef d64view_app_hpp_
#define d64view_app_hpp_

// needed includes
#include "charset.hpp"


// classes

//! application class
class d64view_app: public wxApp
{
public:
   //! called on initialisation
   virtual bool OnInit();

   //! called when exiting
   int OnExit(void);

   //! creates child frame
   wxDocMDIChildFrame* CreateChildFrame(wxDocument* doc, wxView* view, unsigned int type);

   //! creates a block availability map view
   wxView* CreateBamView(wxDocument* doc);

   //! returns c64 charset object
   c64_charset& get_charset(){ return charset; }

protected:
   //! main frame
   wxMDIParentFrame* mainframe;

   //! document manager
   wxDocManager* doc_manager;

   //! doc template for dir view
   wxDocTemplate* dir_view_templ;

   //! doc template for bam view
   wxDocTemplate* bam_view_templ;

   //! charset to use
   c64_charset charset;
};

DECLARE_APP(d64view_app)


#endif // d64view_app_hpp_
