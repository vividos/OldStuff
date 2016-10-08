/*
   d64view - a d64 disk image viewer
   Copyright (c) 2002,2003,2016 Michael Fink
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
