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

   $Id: common.hpp,v 1.13 2003/05/24 11:09:13 vividos Exp $

*/
/*! \file common.hpp

   \brief common includes

   lists all includes that are often needed and should go into a precompiled
   header file.

*/

// include guard
#ifndef d64view_common_hpp_
#define d64view_common_hpp_

// config header
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


// STL includes
#include <vector>
#include <string>


// mingw32 hack: this seemed to be defined somewhere (bad bad!)
#ifdef __MINGW32__
#undef CreateDialog
#undef Yield
#endif

// wxWindows includes

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif


// needed stuff from wxWindows

// doc/view architecture
#if !wxUSE_DOC_VIEW_ARCHITECTURE
#error You must set wxUSE_DOC_VIEW_ARCHITECTURE to 1 in setup.h!
#endif

// multiple document interface
#if !wxUSE_MDI_ARCHITECTURE
#error You must set wxUSE_MDI_ARCHITECTURE to 1 in setup.h!
#endif

// printing
#if !wxUSE_PRINTING_ARCHITECTURE
#error You must set wxUSE_PRINTING_ARCHITECTURE to 1 in setup.h!
#endif


// more common wxWindows includes

#include "wx/docmdi.h"


// c64 stuff

//! a c64 byte
typedef unsigned char c64_byte;

// c64 screen colors

//! background color
const wxColour color_dark_blue = wxColour(0x21,0x1b,0xae);

//! foreground/text color
const wxColour color_light_blue = wxColour(0x5f,0x53,0xfe);

//! color for blocks with errors
const wxColour color_red = wxColour(0xff,0x20,0x20);


#endif // d64view_common_hpp_
