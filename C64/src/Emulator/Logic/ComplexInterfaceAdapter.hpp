//
// Emulator - Simple C64 emulator
// Copyright (C) 2003-2016 Michael Fink
//
/// \file ComplexInterfaceAdapter.hpp Complex interface adapter implementation
//
#pragma once

// includes
#include <set>

namespace C64
{
// forward references
class ICIAPortHandler;

/// \brief CIA implementation
/// \details Implementation of the CIA chip
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

   /// adds a port listener for this CIA
   void AddPortListener(ICIAPortHandler* pPortHandler)
   {
      m_setAllHandler.insert(pPortHandler);
   }

   /// removes a port listener
   void RemovePortListener(ICIAPortHandler* pPortHandler)
   {
      m_setAllHandler.erase(pPortHandler);
   }

private:
   /// all CIA registers
   BYTE m_abRegister[16];

   /// all CIA port handlers
   std::set<ICIAPortHandler*> m_setAllHandler;
};

} // namespace C64
