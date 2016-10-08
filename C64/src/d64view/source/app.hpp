/*
   d64view - a d64 disk image viewer
   Copyright (c) 2002,2003,2016 Michael Fink
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
