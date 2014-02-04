#include "stdafx.h"
#ifndef _WIN32_WCE
#include <ulib/debug/SymbolManager.hpp>
#include <ulib/win32/ErrorMessage.hpp>

using Debug::Symbol;
using Debug::SymLineInfo;
using Debug::SymModuleInfo;
using Debug::SymbolManager;

const unsigned int STACKWALK_MAX_NAMELEN = 1024;

Symbol::Symbol()
:m_apSymbol(std::auto_ptr<IMAGEHLP_SYMBOL64>(
   reinterpret_cast<IMAGEHLP_SYMBOL64*>(
      new BYTE[sizeof(IMAGEHLP_SYMBOL64) + STACKWALK_MAX_NAMELEN]))),
 m_dwDisplacement(0)
{
   ATLASSERT(m_apSymbol.get() != NULL);
   ZeroMemory(m_apSymbol.get(), sizeof(IMAGEHLP_SYMBOL64) + STACKWALK_MAX_NAMELEN);

   m_apSymbol->SizeOfStruct = sizeof(IMAGEHLP_SYMBOL64);
   m_apSymbol->MaxNameLength = STACKWALK_MAX_NAMELEN;
}

CString Symbol::GetName() const
{
   USES_CONVERSION;
   return CString(A2CT(m_apSymbol->Name));
}

SymLineInfo::SymLineInfo()
{
   ZeroMemory(this, sizeof(*this));
   this->SizeOfStruct = sizeof(IMAGEHLP_LINE64);
}

/// \todo add search path
SymbolManager::SymbolManager(HANDLE hProcess)
:m_hProcess(hProcess)
{
   SymSetOptions(SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS);

   // TODO add search path, but could also be set with SymSetSearchPath
   if (FALSE == SymInitialize(m_hProcess, NULL, TRUE))
   {
      if (FALSE == SymInitialize(m_hProcess, NULL, FALSE))
      {
         DWORD dwLastError = GetLastError(); dwLastError;
         ATLTRACE(_T("SymInitialize error 0x%08x: %s\n"),
            dwLastError, Win32::ErrorMessage(dwLastError).Get());
      }
      else
         ATLTRACE(_T("SymInitialize couldn't be started with fInvadeProcess, trying without\n"));
   }
}

SymbolManager::~SymbolManager()
{
   if (FALSE == SymCleanup(m_hProcess))
   {
      DWORD dwLastError = GetLastError(); dwLastError;
      ATLTRACE(_T("SymCleanup error 0x%08x: %s\n"),
         dwLastError, Win32::ErrorMessage(dwLastError).Get());
   }
}

void SymbolManager::GetSymFromAddr(DWORD64 dwAddress, Symbol& symbol)
{
   bool bRet = TRUE == SymGetSymFromAddr64(m_hProcess, dwAddress, &symbol.GetDisplacement(), symbol);

   if (!bRet)
   {
      DWORD dwLastError = GetLastError(); dwLastError;
      ATLTRACE(_T("SymGetSymFromAddr64 error 0x%08x: %s\n"),
         dwLastError, Win32::ErrorMessage(dwLastError).Get());
   }
}

void SymbolManager::GetLineFromAddr(DWORD64 dwAddress, SymLineInfo& lineInfo)
{
   bool bRet = TRUE == SymGetLineFromAddr64(m_hProcess, dwAddress, &lineInfo.GetDisplacement(), lineInfo);

   if (!bRet)
   {
      DWORD dwLastError = GetLastError(); dwLastError;
      ATLTRACE(_T("SymGetLineFromAddr64 error 0x%08x: %s\n"),
         dwLastError, Win32::ErrorMessage(dwLastError).Get());
   }
}

void SymbolManager::GetModuleInfo(DWORD64 dwAddress, SymModuleInfo& moduleInfo)
{
   bool bRet = TRUE == SymGetModuleInfo64(m_hProcess, dwAddress, &moduleInfo);

   if (!bRet)
   {
      DWORD dwLastError = GetLastError(); dwLastError;
      ATLTRACE(_T("SymGetModuleInfo64 error 0x%08x: %s\n"),
         dwLastError, Win32::ErrorMessage(dwLastError).Get());
   }
   else
      _asm nop;
}

#endif // _WIN32_WCE
