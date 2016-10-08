/*
   d64view - a d64 disk image viewer
   Copyright (c) 2002,2003,2016 Michael Fink
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
