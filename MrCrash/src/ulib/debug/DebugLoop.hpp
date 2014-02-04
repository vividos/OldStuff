#pragma once

// includes
#include <set>

namespace Debug
{

/// event handler interface for debug loop
class IDebugEventHandler
{
public:
   ~IDebugEventHandler(){}

   virtual void OnCreateProcess(DWORD dwProcessId, DWORD dwThreadId, LPVOID pImageBase, LPVOID pStartProc, LPCTSTR pszImageName)=0;
   virtual void OnCreateThread(DWORD dwProcessId, DWORD dwThreadId, LPVOID pStartProc)=0;
   virtual void OnExitThread(DWORD dwProcessId, DWORD dwThreadId, DWORD dwExitCode)=0;
   virtual bool OnExitProcess(DWORD dwProcessId, DWORD dwThreadId, DWORD dwExitCode)=0;
   virtual void OnLoadDll(DWORD dwProcessId, DWORD dwThreadId, LPVOID pBaseAddress, LPCTSTR pszDllName)=0;
   virtual void OnUnloadDll(DWORD dwProcessId, DWORD dwThreadId, LPVOID pBaseAddress)=0;
   virtual void OnOutputDebugString(DWORD dwProcessId, DWORD dwThreadId, LPCTSTR pszText)=0;
   virtual void OnException(DWORD dwProcessId, DWORD dwThreadId, EXCEPTION_DEBUG_INFO& debugInfo)=0;
};


/// debugger loop
class DebugLoop
{
public:
   DebugLoop(){}

   void Run();

   void AddObserver(IDebugEventHandler* pObserver)
   {
      m_setEventHandler.insert(pObserver);
   }

   void RemoveObserver(IDebugEventHandler* pObserver)
   {
      m_setEventHandler.erase(pObserver);
   }

protected:
   void OnException(DWORD& dwContinueStatus);
   void OnCreateThread();
   void OnCreateProcess();
   void OnExitThread();
   bool OnExitProcess();
   void OnLoadDll();
   void OnUnloadDll();
   void OnOutputDebugString();

private:
   /// process handle of process to debug
   HANDLE m_hProcess;

   /// debugging event information
   DEBUG_EVENT m_debugEvent;

   /// set with all message handler
   std::set<IDebugEventHandler*> m_setEventHandler;
};


} // namespace Debug
