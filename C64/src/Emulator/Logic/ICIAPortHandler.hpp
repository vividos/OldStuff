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
   virtual void SetDataPort(BYTE portNumber, BYTE value) throw() = 0;

   /// called when data direction register is set
   virtual void SetDataDirection(BYTE portNumber, BYTE value) throw() { portNumber;  value; }

   /// called when data port register is being read
   virtual void ReadDataPort(BYTE portNumber, BYTE& value) const throw() { portNumber; value; }
};

} // namespace C64
