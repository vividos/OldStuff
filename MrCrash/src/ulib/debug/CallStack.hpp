#pragma once

#ifdef _WIN32_WCE
#  error CallStack cannot be used under Windows CE!
#endif

// includes
#include <vector>

#ifdef UNICODE
#define DBGHELP_TRANSLATE_TCHAR
#endif
#pragma warning(push)
#pragma warning(disable: 4091) // 'typedef ': ignored on left of '' when no variable is declared
#include <dbghelp.h>
#pragma warning(pop)

namespace Debug
{
// forward references
class Symbol;
class SymbolManager;

/// infos about a function call
class FunctionCall
{
public:
   FunctionCall(DWORD64 dwIP = 0)
      :m_dwIP(dwIP)
   {
   }

   // get functions
   DWORD64 GetIP() const throw() { return m_dwIP; }
   DWORD64 ModuleBaseAddress() const throw() { return m_dwModuleBaseAddress; }
   const CString& GetFunctionName() const throw() { return m_cszFunctionName; }
   DWORD64 GetFunctionDisplacement() const throw() { return m_dwFunctionDisplacement; }

   const CString& SourceFilename() const throw() { return m_cszSourceFilename; }
   DWORD SourceLineNumber() const throw() { return m_dwSourceLineNumber; }

   // set functions

   void SetSymbol(Symbol& symbol);

   void SetModuleBaseAddress(DWORD64 dwModuleBaseAddress)
   {
      m_dwModuleBaseAddress = dwModuleBaseAddress;
   }

   void SetFunctionName(const CString& cszFunctionName)
   {
      m_cszFunctionName = cszFunctionName;
   }

   void SetSourceFilename(const CString& cszSourceFilename)
   {
      m_cszSourceFilename = cszSourceFilename;
   }

   void SetSourceLineNumber(DWORD dwSourceLineNumber)
   {
      m_dwSourceLineNumber = dwSourceLineNumber;
   }

private:
   DWORD64 m_dwIP; ///< instruction pointer

   DWORD64 m_dwModuleBaseAddress;

   CString m_cszFunctionName; ///< function name
   DWORD64 m_dwFunctionDisplacement; ///< displacement from function start

   CString m_cszSourceFilename;
   DWORD m_dwSourceLineNumber;
};

/// represents a call stack from a stack walk
class CallStack
{
public:
   /// ctor; collects callstack from current function
   CallStack();

   /// ctor; collects callstack from given process and thread
   CallStack(DWORD dwProcessId, DWORD dwThreadId);

   /// returns size of call stack
   size_t GetSize() const { return m_veCallStack.size(); }

   /// returns function call info
   const FunctionCall& GetFunctionCall(size_t uiIndex) const
   {
      ATLASSERT(uiIndex < GetSize());
      return m_veCallStack[uiIndex];
   }

private:
   void CollectCallstack(HANDLE hProcess, HANDLE hThread, CONTEXT& context, SymbolManager& symbolManager);

   bool WalkStack(HANDLE hProcess, HANDLE hThread, DWORD dwMachineType, CONTEXT& context, STACKFRAME64& stackFrame);

   void AddStackFrame(HANDLE hProcess, const STACKFRAME64& stackFrame, Debug::SymbolManager& symManager);

private:
   std::vector<FunctionCall> m_veCallStack;
};

} // namespace Debug
