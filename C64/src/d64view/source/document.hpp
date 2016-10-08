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

   $Id: document.hpp,v 1.9 2003/09/08 20:55:46 vividos Exp $

*/
/*! \file document.hpp

   \brief wxWindows document

   disk document class for doc/view model

*/

// import guard
#ifndef d64view_document_hpp_
#define d64view_document_hpp_

// needed includes
#include "imagebase.hpp"


// classes

//! d64 disk document class
class disk_document: public wxDocument
{
public:
   //! ctor
   disk_document();
   //! dtor
   virtual ~disk_document();

   //! assign operator
   const disk_document& operator=(const disk_document& doc);

   //! returns current disk image
   disk_image_base* get_disk_image(){ return image; }

protected:
   //! called on opening a document
   virtual bool OnOpenDocument(const wxString& filename);

protected:
   //! generic disk image pointer
   disk_image_base* image;

private:
   DECLARE_DYNAMIC_CLASS(disk_document)
};


#endif //d64view_document_hpp_
