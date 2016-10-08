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

   $Id: doctempl.cpp,v 1.3 2003/09/08 20:55:46 vividos Exp $

*/
/*! \file doctempl.cpp

   \brief custom document template

   checking for already openend files

*/

// needed includes
#include "common.hpp"
#include "doctempl.hpp"


// d64view_doc_template methods

wxDocument* d64view_doc_template::CreateDocument(const wxString& path, long flags)
{
   // search through all documents if it is already open
   wxList& doclist = m_documentManager->GetDocuments();

   size_t max = doclist.GetCount();
   for(size_t n=0; n<max; n++)
   {
      wxDocument* doc = reinterpret_cast<wxDocument*>(doclist.Item(n)->GetData());

      if (doc->GetFilename()==path)
      {
         // activate first view, don't create document
         wxView* view = doc->GetFirstView();
         view->Activate(true);
         return NULL;
      }
   }

   return wxDocTemplate::CreateDocument(path,flags);
}
