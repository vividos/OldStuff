/*
   d64view - a d64 disk image viewer
   Copyright (c) 2002,2003,2016 Michael Fink
*/
/*! \file app.cpp

   \brief d64view application class

   application initialization and some useful functions

*/

// needed includes
#include "common.hpp"
#include "app.hpp"
#include "mainframe.hpp"
#include "childframe.hpp"
#include "document.hpp"
#include "doctempl.hpp"
#include "dirview.hpp"
#include "bamview.hpp"


// macros

// implements d64view_app as wxApp class to use
IMPLEMENT_APP(d64view_app)


// external functions

#ifdef __WXMSW__
   extern void init_dde_server(wxDocManager* doc_manager);
   extern void done_dde_server();
#endif


// d64view_app methods

/*! creates the doc manager, creates doc templates for the two views and
    creates the main frame; when files were specified on the command line,
    the documents are opened; for windows, the dde server is started, too. */
bool d64view_app::OnInit()
{
   // create document manager
   doc_manager = new wxDocManager;

   // load charset
   charset.init();

   // create template relating drawing documents to their views
   dir_view_templ = new d64view_doc_template((wxDocManager*)doc_manager,
      "d64 disk image", "*.d64", "", "d64", "d64 disk image", "d64 disk image directory view",
          CLASSINFO(disk_document), CLASSINFO(dir_view));

   bam_view_templ = new d64view_doc_template((wxDocManager*)doc_manager,
      "d64 disk image", "*.d64", "", "d64", "d64 disk image", "d64 disk image bam view",
          CLASSINFO(disk_document), CLASSINFO(bam_view),
          wxTEMPLATE_INVISIBLE);

   // create debugger main frame
   mainframe = new d64view_main_frame(
      doc_manager, (wxFrame*)NULL, "d64view - d64 disk image viewer",
      wxPoint(-1, -1), wxSize(800, 600),
      wxDEFAULT_FRAME_STYLE | wxHSCROLL | wxVSCROLL);

   // show it
   mainframe->Show(TRUE);
   SetTopWindow(mainframe);

   // open all d64 files on the command line
   for(int n=1; n<argc; n++)
      doc_manager->CreateDocument(wxString(argv[n]), wxDOC_SILENT);

#ifdef __WXMSW__
   // start dde server to get "open" commands
   init_dde_server(doc_manager);
#endif

   return true;
}

/*! creates child frame; "type" specifies what type of view window caption
    should be set (0=dir listing, 1=bam view) */
wxDocMDIChildFrame* d64view_app::CreateChildFrame(
   wxDocument* doc, wxView* view, unsigned int type)
{
   wxString viewname(doc->GetFilename());
   wxSize size = wxSize(496, 400);

   switch(type)
   {
   case 0: viewname += " - Directory Listing"; break;
   case 1: viewname += " - Block Availability Map"; size = wxSize(440, 624+3); break;
   default: viewname += " - unknown view type"; break;
   }

   // create child frame
   wxDocMDIChildFrame* childframe =
      new d64view_child_frame(doc_manager, doc, view, mainframe, -1, viewname,
         wxDefaultPosition, size,
         wxDEFAULT_FRAME_STYLE | wxNO_FULL_REPAINT_ON_RESIZE,
         "d64view_childframe");

   return childframe;
}

wxView* d64view_app::CreateBamView(wxDocument* doc)
{
   return bam_view_templ->CreateView(doc);
}

int d64view_app::OnExit(void)
{
#ifdef __WXMSW__
   // end dde server
   done_dde_server();
#endif

   delete doc_manager;
   return 0;
}
