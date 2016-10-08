/*
   d64view - a d64 disk image viewer
   Copyright (c) 2002,2003,2016 Michael Fink
*/
/*! \file childframe.cpp

   \brief child frame class implementation

   common functionality for all child frames in d64view

*/

// needed includes
#include "common.hpp"
#include "mainframe.hpp"
#include "childframe.hpp"
#include "app.hpp"


// macros

BEGIN_EVENT_TABLE(d64view_child_frame, wxDocMDIChildFrame)
   EVT_MENU(MENU_SHOW_BAM, d64view_child_frame::OnMenuShowBam)
   EVT_CLOSE(d64view_child_frame::OnCloseWindow)
END_EVENT_TABLE()


// d64view_child_frame methods

d64view_child_frame::d64view_child_frame(
   wxDocManager* my_doc_manager, wxDocument *doc, wxView *view,
   wxMDIParentFrame* frame, wxWindowID id, const wxString& title,
   const wxPoint& pos, const wxSize& size, long style, const wxString& name)
:wxDocMDIChildFrame(doc, view, frame, id, title, pos, size, style, name),
 doc_manager(my_doc_manager)
{
   // add icon
#ifdef __WXMSW__
   SetIcon(wxIcon("d64_icon"));
#endif

   // create menu bar
   menubar = new wxMenuBar();

   // file menu
   filemenu = new wxMenu();
   filemenu->Append(wxID_OPEN, "&Open", "opens a disk image");
   filemenu->Append(wxID_CLOSE, "&Close", "closes a disk image document");
   filemenu->AppendSeparator();

   filemenu->Append(wxID_PRINT, "&Print...");
   filemenu->Append(wxID_PRINT_SETUP, "Print &Setup...");
   filemenu->Append(wxID_PREVIEW, "Print Pre&view");
   filemenu->AppendSeparator();

   filemenu->Append(wxID_EXIT, "E&xit", "exits the application");

   // append file history
   doc_manager->FileHistoryUseMenu(filemenu);
   doc_manager->FileHistoryAddFilesToMenu(filemenu);

   menubar->Append(filemenu, "&File");

   // disk image menu
   imagemenu = new wxMenu();
   imagemenu->Append(MENU_SHOW_DIR, "show &Directory Listing", "shows Directory Listing");
   imagemenu->Append(MENU_SHOW_BAM, "show &Block Availability Map (BAM)", "shows Block Availability Map");
   menubar->Append(imagemenu, "Disk &Image");

   // info menu
   helpmenu = new wxMenu();
   helpmenu->Append(MENU_INFO_ABOUT, "&About d64view");
   menubar->Append(helpmenu, "&Help");

   SetMenuBar(menubar);
}

bool d64view_child_frame::OnMenuShowBam(wxCommandEvent& event)
{
   wxView* view = wxGetApp().CreateBamView(GetDocument());
   view->OnUpdate(NULL,NULL);

   return TRUE;
}

void d64view_child_frame::OnCloseWindow(wxCloseEvent& event)
{
   wxDocMDIChildFrame::OnCloseWindow(event);

   doc_manager->FileHistoryRemoveMenu(filemenu);
}
