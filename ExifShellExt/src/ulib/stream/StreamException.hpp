//
// ulib - a collection of useful classes
// Copyright (C) 2006-2014 Michael Fink
//
/// \file StreamException.hpp exception used by stream classes
//
#pragma once

// includes
#include <ulib/Exception.hpp>

namespace Stream
{

/// exception that is thrown when an error in a stream method occurs
class StreamException: public Exception
{
public:
   /// ctor; takes source filename and line number
   StreamException(LPCSTR pszSourceFile, UINT uiSourceLine) throw()
      :Exception(pszSourceFile, uiSourceLine)
   {
   }

   /// ctor; takes message and source filename and line number
   StreamException(const CString& cszMessage, LPCSTR pszSourceFile, UINT uiSourceLine) throw()
      :Exception(cszMessage, pszSourceFile, uiSourceLine)
   {
   }
};

} // namespace Stream
