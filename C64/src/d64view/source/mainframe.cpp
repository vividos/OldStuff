/*
   d64view - a d64 disk image viewer
   Copyright (c) 2002,2003,2016 Michael Fink
*/
/*! \file mainframe.cpp

   \brief main frame class implementation

   implements the MDI main frame class.

*/

// needed includes
#include "common.hpp"
#include "mainframe.hpp"
#include <wx/config.h>
#include <wx/fileconf.h>
#include "about.hpp"


// d64view_main_frame event table

BEGIN_EVENT_TABLE(d64view_main_frame, wxDocMDIParentFrame)
   EVT_MENU(MENU_INFO_ABOUT, d64view_main_frame::OnMenuInfoAbout)
END_EVENT_TABLE()


// d64view_main_frame methods

d64view_main_frame::d64view_main_frame(
   wxDocManager* manager, wxFrame* frame, const wxString& title,
   const wxPoint& pos, const wxSize& size, const long style)
:wxDocMDIParentFrame(manager, frame, -1, title, pos, size, style)
{
   // add icon
#ifdef __WXMSW__
   SetIcon(wxIcon("d64view_icon"));
#endif

   // get config file name
#ifdef __WXMSW__
   conffile = "d64view.ini";
#else
   conffile = "~/.d64view.cfg";
#endif

   // load file history
   wxFileConfig cfg("d64view",wxEmptyString,conffile,wxEmptyString,
      wxCONFIG_USE_LOCAL_FILE);

   m_docManager->FileHistoryLoad(cfg);

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

   // use file menu for file history
   m_docManager->FileHistoryUseMenu(filemenu);
   m_docManager->FileHistoryAddFilesToMenu(filemenu);

   menubar->Append(filemenu, "&File");

   // info menu
   infomenu = new wxMenu();
   infomenu->Append(MENU_INFO_ABOUT, "&About d64view");
   menubar->Append(infomenu, "&Help");

   SetMenuBar(menubar);

   Centre(wxBOTH);

   // status bar
   CreateStatusBar();
   SetStatusText("ready.", 0);

   // accept dropped files
   DragAcceptFiles(true);
   SetDropTarget(new d64_drop_target(manager));
}

d64view_main_frame::~d64view_main_frame()
{
   // save file history
   wxFileConfig cfg("d64view",wxEmptyString,conffile,wxEmptyString,
      wxCONFIG_USE_LOCAL_FILE);

   m_docManager->FileHistorySave(cfg);
}

void d64view_main_frame::OnMenuInfoAbout(wxCommandEvent& event)
{
   d64view_about_dlg dlg(this);
   dlg.ShowModal();
}


// d64_drop_target methods

bool d64_drop_target::OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames)
{
   size_t max = filenames.GetCount();

   for (unsigned int n=0; n<max; n++)
      m_docManager->CreateDocument(filenames[n], wxDOC_SILENT);

   return true;
}
