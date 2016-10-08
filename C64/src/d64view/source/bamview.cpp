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

   $Id: bamview.cpp,v 1.7 2003/09/08 20:55:45 vividos Exp $

*/
/*! \file bamview.cpp

   \brief block availability map view

   contains code to init and update the block availability map view

*/

// needed includes
#include "common.hpp"
#include "bamview.hpp"
#include "document.hpp"
#include "app.hpp"


// macros

IMPLEMENT_DYNAMIC_CLASS(bam_view, wxView)


// d64view_view methods

bool bam_view::OnCreate(wxDocument* doc, long flags)
{
   frame = wxGetApp().CreateChildFrame(doc,this,1);

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

void bam_view::OnUpdate(wxView* sender, wxObject* hint)
{
   // get document
   disk_document* d64doc = reinterpret_cast<disk_document*>(GetDocument());

   if (d64doc==NULL)
      return;

   // get disk image object
   disk_image_base* disk = d64doc->get_disk_image();

   // create new canvas
   delete canvas;
   canvas = new c64screen_canvas(21+4, disk->get_num_tracks()+2,
      frame, wxPoint(0, 0), 0);

   canvas->print(3,0, "0    5    10   15   20");

   unsigned int maxtrack = disk->get_num_tracks();
   for(unsigned int track=0; track<maxtrack; track++)
   {
      char buffer[16];

      sprintf(buffer,"% 2.2u",track+1);
      canvas->print(0,track+1, buffer);

      unsigned int maxsect = disk->get_sectors_per_track(track);
      for(unsigned int sect=0; sect<maxsect; sect++)
      {
         unsigned int blocknum = disk->map_to_blocks(track,sect);

         if (disk->get_error_code(blocknum)!=1)
            canvas->set_color(sect+3,track+1,true);

         bool avail = disk->get_block_avail(blocknum);
         canvas->print(sect+3,track+1, !avail? "*" : ".");
      }
   }

   canvas->Refresh();
}

void bam_view::OnDraw(wxDC* dc)
{
   canvas->OnDraw(*dc);
}

bool bam_view::OnClose(bool deleteWindow)
{
   if (!GetDocument()->Close())
      return FALSE;

   SetFrame((wxFrame*)NULL);

   Activate(FALSE);

   if (deleteWindow)
      delete frame;

   return TRUE;
}
