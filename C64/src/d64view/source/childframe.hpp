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

   $Id: childframe.hpp,v 1.5 2003/09/08 20:55:45 vividos Exp $

*/
/*! \file childframe.hpp

   \brief child frame

   generic child frame for the view classes

*/

// include guard
#ifndef d64view_childframe_hpp_
#define d64view_childframe_hpp_


// classes

//! child frame class
class d64view_child_frame: public wxDocMDIChildFrame
{
public:
   //! ctor
   d64view_child_frame(wxDocManager* doc_manager, wxDocument *doc, wxView *view,
      wxMDIParentFrame* frame, wxWindowID id, const wxString& title,
      const wxPoint& pos, const wxSize& size, long style, const wxString& name);

protected:
   // event handler

   //! menu handler: Disk Image -> Show BAM
   bool OnMenuShowBam(wxCommandEvent& event);

   //! called when closing the window
   virtual void OnCloseWindow(wxCloseEvent& event);

protected:
   //! document manager
   wxDocManager* doc_manager;

   //! main frame menu bar
   wxMenuBar* menubar;

   //! file menu
   wxMenu* filemenu;

   //! disk image menu
   wxMenu* imagemenu;

   //! help menu
   wxMenu* helpmenu;

   DECLARE_EVENT_TABLE()
};


#endif // d64view_childframe_hpp_
