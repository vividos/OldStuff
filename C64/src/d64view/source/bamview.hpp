/*
   d64view - a d64 disk image viewer
   Copyright (c) 2002,2003,2016 Michael Fink
*/
/*! \file bamview.hpp

   \brief wxWindows view

   view class for doc/view architecture to show the block availability map

*/

// import guard
#ifndef d64view_bamview_hpp_
#define d64view_bamview_hpp_

// needed includes
#include "canvas.hpp"


// classes

//! view to show disk contents
class bam_view: public wxView
{
public:
   //! ctor
   bam_view(){}

   //! called on creation
   bool OnCreate(wxDocument* doc, long flags);

   //! called when updating the view
   void OnUpdate(wxView* sender, wxObject* hint=(wxObject*)NULL);

   //! called to draw the view
   void OnDraw(wxDC* dc);

   //! called when window is about to be closed
   bool OnClose(bool deleteWindow = TRUE);

protected:
   //! child frame for view
   wxDocMDIChildFrame* frame;

   //! canvas to draw screen contents
   c64screen_canvas* canvas;

   DECLARE_DYNAMIC_CLASS(bam_view)
};


#endif // d64view_bamview_hpp_
