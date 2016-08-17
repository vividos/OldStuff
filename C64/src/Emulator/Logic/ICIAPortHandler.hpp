//
// Emulator - Simple C64 emulator
// Copyright (C) 2003-2016 Michael Fink
//
/// \file ICIAPortHandler.hpp Interface to handle CIA port access
//
#pragma once

// includes

namespace C64
{

/// interface that allows to handle CIA port access
class ICIAPortHandler
{
public:
   /// dtor
   virtual ~ICIAPortHandler() throw()
   {
   }

   /// called when data port register is set
   virtual void SetDataPort(BYTE bValue) throw() = 0;

   /// called when data direction register is set
   virtual void SetDataDirection(BYTE bValue) throw() { (bValue); }

   /// called when data port register is being read; returns true when port byte was returned
   virtual bool ReadDataPort(BYTE&) const throw() { return false; }
};

} // namespace C64
