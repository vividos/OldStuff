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

   $Id: dirview.cpp,v 1.10 2003/09/08 20:55:46 vividos Exp $

*/
/*! \file dirview.cpp

   \brief directory view

   directory view implementation; on updating the view it recrates the view's
   canvas with the directory listing.

*/

// needed includes
#include "common.hpp"
#include "mainframe.hpp"
#include "dirview.hpp"
#include "document.hpp"
#include "app.hpp"


// macros

IMPLEMENT_DYNAMIC_CLASS(dir_view, wxView)

BEGIN_EVENT_TABLE(dir_view, wxView)
END_EVENT_TABLE()


// d64view_view methods

bool dir_view::OnCreate(wxDocument* doc, long flags)
{
   frame = wxGetApp().CreateChildFrame(doc,this,0);

   // canvas is (re)created in OnUpdate()
   canvas = NULL;

#ifdef __X__
   // X seems to require a forced resize
   int x, y;
   frame->GetSize(&x, &y);
   frame->SetSize(-1, -1, x, y);
#endif

   frame->Show(TRUE);
   Activate(TRUE);

   return TRUE;
}

void dir_view::OnUpdate(wxView* sender, wxObject* hint)
{
   // get document
   disk_document* d64doc = reinterpret_cast<disk_document*>(GetDocument());

   if (d64doc==NULL)
      return;

   // get disk image object
   disk_image_base* disk = d64doc->get_disk_image();

   // create new canvas
   delete canvas;
   canvas = new c64screen_canvas(29, disk->get_num_dir_entries()+2,
      frame, wxPoint(0, 0), 0);

   char buffer[64];

   // first line: disk label/id
   std::string disk_label(disk->get_disk_label());
   disk_label.append("                ");

   sprintf(buffer,"\"%.16s\" %.5s",disk_label.c_str(),disk->get_disk_id().c_str());

   canvas->print(0,0,"0 ");
   canvas->set_invert(true);
   canvas->print(2,0,buffer);
   canvas->set_invert(false);

   // directory entries
   unsigned int max = disk->get_num_dir_entries();
   for(unsigned int n=0; n<max; n++)
   {
      const dir_entry& entry = disk->get_dir_entry(n);

      std::string fname(entry.filename);
      fname.append("\"                ");

      // determine type string
      std::string typestr;
      char ch = ' ';

      switch(entry.type&7)
      {
      case 0: typestr = "DEL"; break;
      case 1: typestr = "SEQ"; break;
      case 2: typestr = "PRG"; break;
      case 3: typestr = "USR"; break;
      case 4: typestr = "REL"; break;
      default: typestr = "???"; break;
      }

      if ((entry.type&0x80)==0) typestr+= "*";
      if ((entry.type&0x40)!=0) typestr+= "<";
      if ((entry.type&0x20)!=0) ch = '@';

      // do line and print on canvas
      sprintf(buffer,"%-5u\"%-.16s %c%s",entry.blocks,fname.c_str(),
         ch,typestr.c_str());

      canvas->print(0,n+1,buffer);
   }

   // last line with number of free blocks
   sprintf(buffer,"%u BLOCKS FREE.",disk->get_blocks_free());
   canvas->print(0,max+1,buffer);

   canvas->Refresh();
}

void dir_view::OnActivateView(bool activate, wxView* activeView,
   wxView* deactiveView)
{
   if (activate && frame != NULL)
      frame->SetFocus();
}

void dir_view::OnDraw(wxDC* dc)
{
   canvas->OnDraw(*dc);
}

bool dir_view::OnClose(bool deleteWindow)
{
   if (!GetDocument()->Close())
      return FALSE;

   Activate(FALSE);

   if (deleteWindow)
      delete frame;

   return TRUE;
}
