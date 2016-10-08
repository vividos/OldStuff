/*
   d64view - a d64 disk image viewer
   Copyright (c) 2002,2003,2016 Michael Fink
*/
/*! \file doctempl.hpp

   \brief document template class

   the document template class detects opening an already opened file and
   activates that view instead.

*/

// import guard
#ifndef d64view_doctempl_hpp_
#define d64view_doctempl_hpp_

// needed includes


// classes

//! document template class
class d64view_doc_template: public wxDocTemplate
{
public:
   //! ctor
   d64view_doc_template(wxDocManager* manager, const wxString& descr,
      const wxString& filter, const wxString& dir, const wxString& ext,
      const wxString& docTypeName, const wxString& viewTypeName,
      wxClassInfo* docClassInfo = NULL, wxClassInfo* viewClassInfo = NULL,
      long flags = wxDEFAULT_TEMPLATE_FLAGS)
      :wxDocTemplate(manager,descr,filter,dir,ext,docTypeName,viewTypeName,
      docClassInfo,viewClassInfo,flags)
   {
   }

   //! called when creating new documents
   virtual wxDocument* CreateDocument(const wxString& path, long flags = 0);
};


#endif
