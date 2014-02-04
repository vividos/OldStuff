//
// ulib - a collection of useful classes
// Copyright (C) 2006,2007,2008,2009 Michael Fink
//
/// \file DecoratedName.hpp decorated name resolution
//
#pragma once

// includes

#ifdef _WIN32_WCE
#  error DecoratedName cannot be used under Windows CE!
#endif

namespace Debug
{

/// Resolves decorated names
/** A decorated name is a function, class or variable name that contains extra
    characters to describe the complete type. The decorated name is used
    internally by the compiler and is stored in debug symbol info.
*/
class DecoratedName
{
public:
   /// ctor
   DecoratedName(const CString& cszDecoratedName) throw()
      :m_cszDecoratedName(cszDecoratedName)
   {
   }

   /// returns undecorated name of given decorated name
   CString GetUndecorated() const;

private:
   /// decorated name
   CString m_cszDecoratedName;
};

} // namespace Debug
