#include "stdafx.h"
#include <ulib/wince/HardwareKeyManager.hpp>
#include <algorithm>

std::auto_ptr<CHardwareKeyManager> CHardwareKeyManager::s_apHardwareManager;

#define HC_ACTION 0
#define WH_KEYBOARD_LL 20

CHardwareKeyManager::CHardwareKeyManager()
:m_pCallback(NULL),
 m_hHook(NULL)
{
   // assume coredll.dll is already loaded and just get a module address
   HMODULE hModuleCoredll = ::GetModuleHandle(_T("coredll.dll"));
   ATLASSERT(hModuleCoredll != NULL);

   m_fnSetWindowsHookExW =
      reinterpret_cast<T_fnSetWindowsHookExW>(::GetProcAddress(hModuleCoredll, _T("SetWindowsHookExW")));
   m_fnCallNextHookEx =
      reinterpret_cast<T_fnCallNextHookEx>(::GetProcAddress(hModuleCoredll, _T("CallNextHookEx")));
   m_fnUnhookWindowsHookEx =
      reinterpret_cast<T_fnUnhookWindowsHookEx>(::GetProcAddress(hModuleCoredll, _T("UnhookWindowsHookEx")));
   m_fnRegisterHotKey =
      reinterpret_cast<T_fnRegisterHotKey>(::GetProcAddress(hModuleCoredll, _T("RegisterHotKey")));
   m_fnUnregisterFunc1 =
      reinterpret_cast<T_fnUnregisterFunc1>(::GetProcAddress(hModuleCoredll, _T("UnregisterFunc1")));
}

CHardwareKeyManager::~CHardwareKeyManager()
{
   Stop();
   m_fnCallNextHookEx = NULL;
}

CHardwareKeyManager& CHardwareKeyManager::GetInstance()
{
   if (s_apHardwareManager.get() == NULL)
      s_apHardwareManager = std::auto_ptr<CHardwareKeyManager>(new CHardwareKeyManager);

   return *s_apHardwareManager.get();
}

void CHardwareKeyManager::AddHotkey(UINT uiVKHotkey)
{
   m_setRegisteredHotkeys.insert(uiVKHotkey);
}

void CHardwareKeyManager::ResetHotkeys()
{
   m_setRegisteredHotkeys.clear();
}

bool CHardwareKeyManager::Start(HINSTANCE hInstance, HWND hWnd)
{
   ATLASSERT(m_fnSetWindowsHookExW != NULL &&
      m_fnCallNextHookEx != NULL &&
      m_fnRegisterHotKey != NULL &&
      m_fnUnregisterFunc1 != NULL);

   // reregister all keys
   for (UINT vk = 0xc1; vk <= 0xcf; vk++)
   {
      m_fnUnregisterFunc1(MOD_WIN, vk);
      m_fnRegisterHotKey(hWnd, vk, MOD_WIN, vk);
   }

   // establish keyboard hook
   m_hHook = m_fnSetWindowsHookExW(WH_KEYBOARD_LL, &HookProc, hInstance, 0);
   if (m_hHook == NULL)
      ATLTRACE(_T("CHardwareKeyManager: failed to hook keyboard"));

   return m_hHook != NULL;
}

void CHardwareKeyManager::Stop()
{
   ATLASSERT(m_fnUnhookWindowsHookEx != NULL &&
      m_fnUnregisterFunc1 != NULL);

   m_fnUnhookWindowsHookEx(m_hHook);

   for (UINT vk = 0xc1; vk <= 0xcf; vk++)
      m_fnUnregisterFunc1(MOD_WIN, vk);

   m_hHook = NULL;
}

bool CHardwareKeyManager::HookCallback(UINT uVKcode, bool bKeyDown, bool bSysKey)
{
   // no callback? then we don't handle the key
   if (m_pCallback == NULL)
      return true;

   // check if the set of keys to handle is empty
   if (m_setRegisteredHotkeys.size() != 0)
   {
      // key is in the set?
      if (m_setRegisteredHotkeys.find(uVKcode) == m_setRegisteredHotkeys.end())
         return false; // no, so don't handle the key
   }

   // let the callback handle the key
   return m_pCallback->OnHardwareKey(uVKcode, bKeyDown, bSysKey);
}

/// \internal data struct used in the HookProc function
typedef struct
{
   DWORD vkCode;     ///< virtual key code
   DWORD scanCode;   ///< scan code
   DWORD flags;      ///< flags
   DWORD time;       ///< time point
   ULONG_PTR dwExtraInfo;  ///< extra info
} KBDLLHOOKSTRUCT, *PKBDLLHOOKSTRUCT;

LRESULT CHardwareKeyManager::HookProc(int iCode, WPARAM wParam, LPARAM lParam)
{
   ATLASSERT(s_apHardwareManager.get() != NULL);

   ATLTRACE(_T("hookproc: code=%04x, wparam=%04x, lparam=%p"), iCode, wParam, lParam);

   bool bHandled = false;

   if (iCode == HC_ACTION)
   {
      // keyboard action
      KBDLLHOOKSTRUCT* pDllHook = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);

      ATLTRACE(_T(", vc=%04x sc=%04x flags=%04x, time=%08x, extraInfo=%08x\n"),
         pDllHook->vkCode, pDllHook->scanCode,
         pDllHook->flags, pDllHook->time, pDllHook->dwExtraInfo);

      // note: HookCallback returns true when the key should be processed by the system
      if (wParam == WM_KEYDOWN || wParam == WM_KEYUP ||
         wParam == WM_SYSKEYDOWN || wParam == WM_SYSKEYUP)
      {
         bool bRet = s_apHardwareManager->HookCallback(
            pDllHook->vkCode,
            wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN,
            wParam == WM_SYSKEYDOWN || wParam == WM_SYSKEYUP);

         bHandled = !bRet;
      }
   }
   else
      ATLTRACE(_T("\n"));

   if (bHandled)
      return TRUE; // key was handled
   else
   {
      // call next hook in chain
      return s_apHardwareManager->m_fnCallNextHookEx(
         s_apHardwareManager->m_hHook, iCode, wParam, lParam);
   }
}
