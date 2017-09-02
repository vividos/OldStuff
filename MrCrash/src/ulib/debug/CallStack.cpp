#include "stdafx.h"
#ifndef _WIN32_WCE
#include <ulib/debug/CallStack.hpp>
#include <ulib/debug/SymbolManager.hpp>
#include <ulib/debug/DecoratedName.hpp>
#include <boost/static_assert.hpp>

// borrowed ideas from http://www.codeproject.com/threads/StackWalker.asp
// debugging tools for windows available here:
// http://www.microsoft.com/whdc/devtools/debugging/default.mspx

#pragma comment(lib, "dbghelp.lib")

using Debug::FunctionCall;
using Debug::CallStack;

void FunctionCall::SetSymbol(Debug::Symbol& symbol)
{
   m_cszFunctionName = symbol.GetName();
   m_dwFunctionDisplacement = symbol.GetDisplacement();
}

// from StackWalker.h:
//
// The "ugly" assembler-implementation is needed for systems before XP
// If you have a new PSDK and you only compile for XP and later, then you can use 
// the "RtlCaptureContext"
// Currently there is no define which determines the PSDK-Version... 
// So we just use the compiler-version (and assumes that the PSDK is 
// the one which was installed by the VS-IDE)

// INFO: If you want, you can use the RtlCaptureContext if you only target XP and later...
//       But I currently use it in x64/IA64 environments...
#if defined(_M_IX86) || (_WIN32_WINNT <= 0x0500) && (_MSC_VER < 1400)

//#if defined(_M_IX86)
#ifdef CURRENT_THREAD_VIA_EXCEPTION
// TODO: The following is not a "good" implementation, 
// because the callstack is only valid in the "__except" block...
#define GET_CURRENT_CONTEXT(c, contextFlags) \
  do { \
    memset(&c, 0, sizeof(CONTEXT)); \
    EXCEPTION_POINTERS *pExp = NULL; \
    __try { \
      throw 0; \
    } __except( ( (pExp = GetExceptionInformation()) ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_EXECUTE_HANDLER)) {} \
    if (pExp != NULL) \
      memcpy(&c, pExp->ContextRecord, sizeof(CONTEXT)); \
      c.ContextFlags = contextFlags; \
  } while(0);
#else
// The following should be enough for walking the callstack...
#define GET_CURRENT_CONTEXT(c, contextFlags) \
  do { \
    memset(&c, 0, sizeof(CONTEXT)); \
    c.ContextFlags = contextFlags; \
    __asm    call x \
    __asm x: pop eax \
    __asm    mov c.Eip, eax \
    __asm    mov c.Ebp, ebp \
    __asm    mov c.Esp, esp \
  } while(0);
#endif

#else

extern "C" void __stdcall RtlCaptureContext(CONTEXT*);

// The following is defined for x86 (XP and higher), x64 and IA64:
#define GET_CURRENT_CONTEXT(c, contextFlags) \
  do { \
    memset(&c, 0, sizeof(CONTEXT)); \
    c.ContextFlags = contextFlags; \
    RtlCaptureContext(&c); \
} while(0);
#endif


#pragma warning(push)
#pragma warning(disable: 4748) // /GS can not protect parameters and local variables from local buffer overrun because optimizations are disabled in function
#pragma warning(disable: 4740) // flow in or out of inline asm code suppresses global optimization

CallStack::CallStack()
{
   DWORD dwProcessId = GetCurrentProcessId();
   HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessId);

   DWORD dwThreadId = GetCurrentThreadId();
   HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, dwThreadId);

   SymbolManager symbolManager(hProcess);

   // get current context
   CONTEXT context;
   GET_CURRENT_CONTEXT(context, CONTEXT_FULL)

   CollectCallstack(hProcess, hThread, context, symbolManager);

   CloseHandle(hThread);
   CloseHandle(hProcess);
}

#pragma warning(pop)

/// suspends thread for acquiring context
CallStack::CallStack(DWORD dwProcessId, DWORD dwThreadId)
{
   HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessId);
   HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, dwThreadId);

   SymbolManager symbolManager(hProcess);

   // get current context
   CONTEXT context;
   ZeroMemory(&context, sizeof(CONTEXT));
   context.ContextFlags = CONTEXT_FULL;

   SuspendThread(hThread);

   if (GetThreadContext(hThread, &context) == FALSE)
   {
      ResumeThread(hThread);
      return;
   }

   CollectCallstack(hProcess, hThread, context, symbolManager);

   ResumeThread(hThread);

   CloseHandle(hThread);
   CloseHandle(hProcess);
}

void CallStack::CollectCallstack(HANDLE hProcess, HANDLE hThread, CONTEXT& context, Debug::SymbolManager& symbolManager)
{
   STACKFRAME64 stackFrame;
   ZeroMemory(&stackFrame, sizeof(stackFrame));

   // detect machine type and set registers here
#if defined(_M_IX86)
   DWORD dwMachineType = IMAGE_FILE_MACHINE_I386;
   stackFrame.AddrPC.Offset = context.Eip;
   stackFrame.AddrFrame.Offset = context.Ebp;
   stackFrame.AddrStack.Offset = context.Esp;

#elif defined(_M_IA64)
   DWORD dwMachineType = IMAGE_FILE_MACHINE_IA64;
   stackFrame.AddrPC.Offset = context.StIIP;
	stackFrame.AddrStack.Offset = context.IntSp;
	stackFrame.AddrFrame.Offset = context.RsBSP;

   stackFrame.AddrBStore.Offset = context.RsBSP;
   stackFrame.AddrBStore.Mode = AddrModeFlat;

#elif defined(_M_AMD64)
   DWORD dwMachineType = IMAGE_FILE_MACHINE_AMD64;
   stackFrame.AddrPC.Offset = context.Rip;
	stackFrame.AddrStack.Offset = context.Rsp;
	stackFrame.AddrFrame.Offset = context.Rbp;

#else
#  error "platform not supported!"
#endif

   stackFrame.AddrPC.Mode = AddrModeFlat;
   stackFrame.AddrFrame.Mode = AddrModeFlat;
   stackFrame.AddrStack.Mode = AddrModeFlat;

   while (WalkStack(hProcess, hThread, dwMachineType, context, stackFrame) && stackFrame.AddrPC.Offset != 0)
   {
      // add stack frame to callstack
      AddStackFrame(hProcess, stackFrame, symbolManager);
   }
}

bool CallStack::WalkStack(HANDLE hProcess, HANDLE hThread, DWORD dwMachineType, CONTEXT& context, STACKFRAME64& stackFrame)
{
   bool bRet = TRUE == StackWalk64(
      dwMachineType,
      hProcess, hThread, // process and thread
      &stackFrame, // stack frame to be filled
      &context, // context to use to extract callstack infos
      NULL, // ReadProcessMemoryProc64
      SymFunctionTableAccess64,
      SymGetModuleBase64,
      NULL); // TranslateAddressProc64

   return bRet;
}

void CallStack::AddStackFrame(HANDLE hProcess, const STACKFRAME64& stackFrame, Debug::SymbolManager& symManager)
{
   FunctionCall fc(stackFrame.AddrPC.Offset);

   // get function name
   Symbol symbol;
   symManager.GetSymFromAddr(stackFrame.AddrPC.Offset, symbol);

   fc.SetSymbol(symbol);

   // get line info
   SymLineInfo lineInfo;
   symManager.GetLineFromAddr(stackFrame.AddrPC.Offset, lineInfo);

   fc.SetSourceFilename(lineInfo.GetFilename());
   fc.SetSourceLineNumber(lineInfo.GetLineNumber());

   // first, get module base address
   DWORD64 moduleBaseAddress = SymGetModuleBase64(hProcess, stackFrame.AddrPC.Offset);

   // get module info
   SymModuleInfo moduleInfo;
   moduleInfo.SizeOfStruct = sizeof(IMAGEHLP_MODULE64);
   symManager.GetModuleInfo(moduleBaseAddress, moduleInfo);

   fc.SetModuleBaseAddress(moduleBaseAddress);

   m_veCallStack.push_back(fc);

   // check if module info members are of same type as TCHAR; when not, the
   // define DBGHELP_TRANSLATE_TCHAR is missing somewhere
//   BOOST_STATIC_ASSERT(sizeof(moduleInfo.ModuleName[0]) == sizeof(TCHAR));

   ATLTRACE(_T("%s(%u): frame %u: ip=%08llx bp=%08llx sp=%08llx name=%s + 0x%llx\n")
      _T("   module=%s, image=%s loadedImage=%s pdb=%s\n"),
      lineInfo.GetFilename().GetString(),
      lineInfo.GetLineNumber(),
      m_veCallStack.size(),
      stackFrame.AddrPC.Offset,
      stackFrame.AddrFrame.Offset,
      stackFrame.AddrStack.Offset,
      symbol.GetName().GetString(),
      symbol.GetDisplacement(),

      moduleInfo.ModuleName,
      moduleInfo.ImageName,
      moduleInfo.LoadedImageName,
      moduleInfo.LoadedPdbName
      );
}

#endif // _WIN32_WCE
