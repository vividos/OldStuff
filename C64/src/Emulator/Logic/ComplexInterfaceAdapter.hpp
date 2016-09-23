//
// Emulator - Simple C64 emulator
// Copyright (C) 2003-2016 Michael Fink
//
/// \file ComplexInterfaceAdapter.hpp Complex interface adapter implementation
//
#pragma once

// includes

namespace C64
{
// forward references
class ICIAPortHandler;

/// \brief CIA implementation
/// \details Implementation of the CIA chip, MOS 6526
class ComplexInterfaceAdapter
{
public:
   /// ctor
   ComplexInterfaceAdapter();
   /// dtor
   ~ComplexInterfaceAdapter() throw()
   {
   }

   /// sets a CIA register, and notifies port listener
   void SetRegister(BYTE bRegister, BYTE bValue);

   /// reads register value
   BYTE ReadRegister(BYTE bRegister);

   /// sets a port listener for this CIA
   void SetPortListener(ICIAPortHandler* portHandler)
   {
      m_portHandler = portHandler;
   }

private:
   /// all CIA registers
   BYTE m_abRegister[16];

   /// port handler for the registers 0 and 1; may be nullptr
   ICIAPortHandler* m_portHandler;
};

} // namespace C64
