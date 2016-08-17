//
// Emulator - Simple C64 emulator
// Copyright (C) 2003-2016 Michael Fink
//
/// \file ComplexInterfaceAdapter.cpp Complex interface adapter implementation
//

// includes
#include "StdAfx.h"
#include "ComplexInterfaceAdapter.hpp"
#include "ICIAPortHandler.hpp"

using C64::ComplexInterfaceAdapter;

ComplexInterfaceAdapter::ComplexInterfaceAdapter()
{
   memset(m_abRegister, 0, sizeof(m_abRegister));

   m_abRegister[0] = 0xff;
   m_abRegister[1] = 0xff;
}

void ComplexInterfaceAdapter::SetRegister(BYTE bRegister, BYTE bValue)
{
   ATLASSERT(bRegister < sizeof(m_abRegister)/sizeof(*m_abRegister));
   m_abRegister[bRegister & 15] = bValue;

   if (bRegister == 0 || bRegister == 1)
   {
      std::set<ICIAPortHandler*>::iterator iter, stop;
      iter = m_setAllHandler.begin();
      stop = m_setAllHandler.end();

      for(;iter != stop; iter++)
         (*iter)->SetDataPort(bValue);
   }
}

BYTE ComplexInterfaceAdapter::ReadRegister(BYTE bRegister)
{
   ATLASSERT(bRegister < sizeof(m_abRegister)/sizeof(*m_abRegister));

   return m_abRegister[bRegister & 15];
}
