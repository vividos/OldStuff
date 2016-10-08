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

   $Id: dirview.hpp,v 1.9 2003/09/08 20:55:46 vividos Exp $

*/
/*! \file dirview.hpp

   \brief directory listing view

   dir view class for doc/view architecture

*/

// import guard
#ifndef d64view_dirview_hpp_
#define d64view_dirview_hpp_

// needed includes
#include "canvas.hpp"


// classes

//! view to show d64 directory contents
class dir_view: public wxView
{
public:
   //! ctor
   dir_view(){}

protected:
   //! called on creation
   virtual bool OnCreate(wxDocument* doc, long flags);

   //! called when updating the view
   virtual void OnUpdate(wxView* sender, wxObject* hint=(wxObject*)NULL);

   //! called when view is activated
   virtual void OnActivateView(bool activate, wxView* activeView,
      wxView* deactiveView);

   //! called to draw the view
   virtual void OnDraw(wxDC* dc);

   //! called when window is about to be closed
   virtual bool OnClose(bool deleteWindow = TRUE);

protected:
   //! child frame for view
   wxDocMDIChildFrame* frame;

   //! canvas to draw screen contents
   c64screen_canvas* canvas;

   DECLARE_EVENT_TABLE()

   DECLARE_DYNAMIC_CLASS(dir_view)
};


#endif // d64view_dirview_hpp_
