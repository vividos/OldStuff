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

   m_abRegister[0] = 0xff; // port A
   m_abRegister[1] = 0xff; // port B

   m_abRegister[2] = 0xff; // direction port A
   m_abRegister[3] = 0x00; // direction port B
}

void ComplexInterfaceAdapter::SetRegister(BYTE bRegister, BYTE bValue)
{
   ATLASSERT(bRegister < sizeof(m_abRegister)/sizeof(*m_abRegister));

   if (m_portHandler &&
      bRegister >= 0 &&
      bRegister <= 3)
   {
      if (bRegister == 0 || bRegister == 1)
      {
         m_portHandler->SetDataPort(bRegister, bValue);
      }

      if (bRegister == 2 || bRegister == 3)
      {
         m_portHandler->SetDataDirection(bRegister & 2, bValue);
      }
   }

   m_abRegister[bRegister & 15] = bValue;
}

BYTE ComplexInterfaceAdapter::ReadRegister(BYTE bRegister)
{
   ATLASSERT(bRegister < sizeof(m_abRegister)/sizeof(*m_abRegister));

   if (m_portHandler &&
      (bRegister == 0 || bRegister == 1))
   {
      BYTE bValue = 0;
      m_portHandler->ReadDataPort(bRegister, bValue);

      m_abRegister[bRegister & 15] = bValue;
   }

   return m_abRegister[bRegister & 15];
}
