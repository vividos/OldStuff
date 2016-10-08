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

   $Id: doctempl.hpp,v 1.4 2003/09/08 20:55:46 vividos Exp $

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
