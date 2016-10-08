/*
   d64view - a d64 disk image viewer
   Copyright (c) 2002,2003,2016 Michael Fink
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
