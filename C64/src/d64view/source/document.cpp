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

   $Id: document.cpp,v 1.11 2003/09/08 20:55:46 vividos Exp $

*/
/*! \file document.cpp

   \brief document implementation

   document framework stuff

*/

// needed includes
#include "common.hpp"
#include "document.hpp"

// image formats
#include "d64image.hpp"


// macros

IMPLEMENT_DYNAMIC_CLASS(disk_document, wxDocument)


// disk_document methods

disk_document::disk_document()
:image(NULL)
{
}

disk_document::~disk_document()
{
   delete image;
   image = NULL;
}

bool disk_document::OnOpenDocument(const wxString& filename)
{
   // create new image (depending on file extension)
   image = new d64_image;

   // load image
   if (!image->load_image(filename))
   {
      // warn about failure
      wxString msg;
      msg.Printf("The file %s could not be opened.",filename.c_str());

      wxMessageBox(msg,"d64view");
      return false;
   }

   Modify(FALSE);
   UpdateAllViews();

   return true;
}
