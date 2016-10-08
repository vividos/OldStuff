/*
   d64view - a d64 disk image viewer
   Copyright (c) 2002,2003,2016 Michael Fink
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
