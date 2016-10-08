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

   $Id: ddeserv.cpp,v 1.4 2003/05/19 21:08:15 vividos Exp $

*/
/*! \file ddeserv.cpp

   \brief DDE server implementation

   functions to start a DDE server to get [open("<filename>")] commands sent
   from the explorer; that allows for opening disk images when d64view already
   runs. only works for Windows.

*/

// needed includes
#include "common.hpp"

#ifdef __WXMSW__ // only for windows apps

#include <wx/dde.h>


// classes

//! dde server waiting for "ddeexec" commands
class dde_server: public wxDDEServer
{
public:
   //! called to accept a dde connection
   wxConnectionBase *OnAcceptConnection(const wxString& topic);
};

//! dde server connection
class dde_connection: public wxDDEConnection
{
public:
   //! ctor
   dde_connection(char* buffer, int size):wxDDEConnection(buffer,size){}

   //! called on receiving an execute command
   bool OnExecute(const wxString& topic, char *data, int size, wxIPCFormat format);
};


// global variables

static dde_server* m_server;
static char exec_buffer[256+256];
static wxDocManager* doc_manager;

// global functions

//! starts dde server to receive system commands
void init_dde_server(wxDocManager* the_doc_manager)
{
   wxString service = "d64view";

   m_server = new dde_server;
   m_server->Create(service);

   doc_manager = the_doc_manager;
}

void done_dde_server()
{
   delete m_server;
   doc_manager = NULL;
}


// dde_server methods

wxConnectionBase *dde_server::OnAcceptConnection(const wxString& topic)
{
   // system command?
   if (topic=="System")
      return new dde_connection(exec_buffer,WXSIZEOF(exec_buffer));
   else
      return NULL;
}


// dde_connection methods

bool dde_connection::OnExecute(const wxString& WXUNUSED(topic),
   char *data, int WXUNUSED(size), wxIPCFormat WXUNUSED(format))
{
   wxString cmd(data);

   // open command?
   if (cmd.Find("[open(\"")==0)
   {
      // get filename
      cmd.Remove(0,7);
      cmd.Remove(cmd.Length()-3);

      // load document
      doc_manager->CreateDocument(cmd, wxDOC_SILENT);
   }

   return TRUE;
}

#endif // __WXMSW__
