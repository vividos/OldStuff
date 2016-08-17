//
// Emulator - Simple C64 emulator
// Copyright (C) 2003-2016 Michael Fink
//
/// \file CIAMemoryAdapter.cpp Memory adapter for ComplexInterfaceAdapter
//

// includes
#include "StdAfx.h"
#include "CIAMemoryAdapter.hpp"
#include "ComplexInterfaceAdapter.hpp"

using C64::CIAMemoryAdapter;

BYTE CIAMemoryAdapter::Peek(WORD wAddr) throw()
{
   if (wAddr >= m_wBaseAddr && wAddr < m_wBaseAddr + 0x0100)
   {
      ATLTRACE(_T("CIA register read at $%04x\n"), wAddr);

      return m_cia.ReadRegister(static_cast<BYTE>(wAddr & 0x0f));
   }
   else
      ATLASSERT(false);

   return 0;
}

void CIAMemoryAdapter::Poke(WORD wAddr, BYTE bValue) throw()
{
   if (wAddr >= m_wBaseAddr && wAddr < m_wBaseAddr + 0x0100)
   {
      ATLTRACE(_T("CIA register write at $%04x = $%02x\n"), wAddr, bValue);

      m_cia.SetRegister(static_cast<BYTE>(wAddr & 0x0f), bValue);
   }
   else
      ATLASSERT(false);
}
