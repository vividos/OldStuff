#pragma once

// includes
#include <memory> // for std::auto_ptr

#ifdef UNICODE
#define DBGHELP_TRANSLATE_TCHAR
#endif
#include <dbghelp.h>

namespace Debug
{

/// debug symbol
class Symbol
{
public:
   Symbol();

   CString GetName() const;

   DWORD64 GetDisplacement() const { return m_dwDisplacement; }

   DWORD64& GetDisplacement(){ return m_dwDisplacement; }

   operator PIMAGEHLP_SYMBOL64() const { return m_apSymbol.get(); }

private:
  std::auto_ptr<IMAGEHLP_SYMBOL64> m_apSymbol;
  DWORD64 m_dwDisplacement;
};

/// \brief symbol line info
/// wraps win32 IMAGEHLP_LINE64 struct
class SymLineInfo: public IMAGEHLP_LINE64
{
public:
   SymLineInfo();

   CString GetFilename() const{ return this->FileName; }

   DWORD GetLineNumber() const { return this->LineNumber; }

   DWORD GetDisplacement() const { return m_dwDisplacement; }

   DWORD& GetDisplacement(){ return m_dwDisplacement; }

   operator PIMAGEHLP_LINE64(){ return this; }

private:
   DWORD m_dwDisplacement;
};

/// \brief symbol module info
/// wraps win32 IMAGEHLP_MODULE64 struct
class SymModuleInfo: public IMAGEHLP_MODULE64
{
public:
   /// ctor
   SymModuleInfo()
   {
      ZeroMemory(this, sizeof(IMAGEHLP_MODULE64));
      this->SizeOfStruct = sizeof(IMAGEHLP_MODULE64);
   }
};

/// symbol manager
class SymbolManager
{
public:
   /// ctor; starts symbol manager using process handle
   SymbolManager(HANDLE hProcess);
   /// dtor
   ~SymbolManager();

   /// returns symbol manager option value
   DWORD GetOptions()
   {
      return SymGetOptions();
   }

   /// sets symbol manager option
   void SetOptions(DWORD dwOptions)
   {
      SymSetOptions(dwOptions);
   }

   /// returns symbol from given address
   void GetSymFromAddr(DWORD64 dwAddress, Symbol& symbol);

   /// returns line info from given address
   void GetLineFromAddr(DWORD64 dwAddress, SymLineInfo& lineInfo);

   /// returns module info for module starting at given address
   void GetModuleInfo(DWORD64 dwAddress, SymModuleInfo& moduleInfo);

private:
   /// process handle of process that is being examined
   HANDLE m_hProcess;
};

} // namespace Debug
