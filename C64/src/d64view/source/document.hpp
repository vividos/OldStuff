/*
   d64view - a d64 disk image viewer
   Copyright (c) 2002,2003,2016 Michael Fink
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
