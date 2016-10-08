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

   $Id: mainframe.hpp,v 1.9 2003/09/08 20:56:31 vividos Exp $

*/
/*! \file mainframe.hpp

   \brief main frame for d64view application

   MDI (multiple document interface) frame for all view subframes that show
   the contents of d64 images.

*/

// import guard
#ifndef d64view_mainframe_hpp_
#define d64view_mainframe_hpp_

// needed includes
#include "wx/dnd.h"


// menu commands
enum
{
   MENU_SHOW_DIR = 1,
   MENU_SHOW_BAM,
   MENU_INFO_ABOUT
};


// classes

//! main frame class
class d64view_main_frame: public wxDocMDIParentFrame
{
public:
   //! ctor
   d64view_main_frame(
      wxDocManager* manager, wxFrame* frame,
      const wxString& title,const wxPoint& pos, const wxSize& size,
      const long style);

   //! dtor
   virtual ~d64view_main_frame();

protected:
   // message handler

   //! menu handler: Info -> About
   void OnMenuInfoAbout(wxCommandEvent& event);

protected:
   //! main frame menu bar
   wxMenuBar* menubar;

   //! file menu
   wxMenu* filemenu;

   //! info menu
   wxMenu* infomenu;

   //! name of config file
   wxString conffile;

   DECLARE_EVENT_TABLE()
};


//! d64 file drop target class
class d64_drop_target: public wxFileDropTarget
{
public:
   //! ctor
   d64_drop_target(wxDocManager* manager):m_docManager(manager){}

   //! called when files are dropped on the main frame
   virtual bool OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames);

protected:
   //! document manager
   wxDocManager* m_docManager;
};


#endif // d64view_mainframe_hpp_
