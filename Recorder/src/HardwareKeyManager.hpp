//
// Recorder - a GPS logger app for Windows Mobile
// Copyright (C) 2006-2019 Michael Fink
//
/// \file HardwareKeyManager.hpp Hardware key manager
//
#pragma once

#include <memory>
#include <map>
#include <set>
#include <boost/scoped_ptr.hpp>

/// callback interface for hardware key events
class IHardwareKeyCallback
{
public:
   /// dtor
   virtual ~IHardwareKeyCallback() throw() {}

   /// \brief called when a hotkey is returned
   /// \param uVKCode virtual keycode of the hardware key being pressed
   /// \param bKeyDown indicates if it was a key down event
   /// \param bSysKey indicates if a system key was pressed
   /// \retval true return true when the key should be processed by the OS
   /// \retval false return false when the key has already been processed
   virtual bool OnHardwareKey(UINT uVKCode, bool bKeyDown, bool bSysKey) = 0;
};


typedef LRESULT(CALLBACK* HOOKPROC)(int code, WPARAM wParam, LPARAM lParam);
typedef HHOOK(WINAPI* T_fnSetWindowsHookExW)(int, HOOKPROC, HINSTANCE, DWORD);
typedef LRESULT(WINAPI* T_fnCallNextHookEx)(HHOOK, int, WPARAM, LPARAM);
typedef LRESULT(WINAPI* T_fnUnhookWindowsHookEx)(HHOOK);
typedef BOOL(WINAPI* T_fnRegisterHotKey)(HWND, int, UINT, UINT);
typedef BOOL(WINAPI* T_fnUnregisterFunc1)(UINT, UINT);

/// \brief Hardware key manager
/// Allows to get notified when a user presses a hardware key on the Pocket PC.
/// The manager has a set of hotkeys that it recognizes for being processed.
/// When the set is empty, all keys are processed. All keys are handled through
/// a callback that can be set.
class CHardwareKeyManager
{
public:
   /// returns
   static CHardwareKeyManager& GetInstance();

   /// dtor
   ~CHardwareKeyManager();

   /// sets callback
   void SetCallback(IHardwareKeyCallback* pCallback) { m_pCallback = pCallback; }

   /// starts listening for hardware hotkey presses
   bool Start(HINSTANCE hInstance, HWND hWnd);

   /// stops hardware hotkey monitoring
   void Stop();

   /// adds hotkey to list of hotkeys being recognized
   void AddHotkey(UINT uiVKHotkey);

   /// reset list of hotkeys being recognized; when the list is empty, all keys are recognized
   void ResetHotkeys();

private:
   /// ctor
   CHardwareKeyManager();

   /// keyboard hook procedure
   static LRESULT HookProc(int code, WPARAM wParam, LPARAM lParam);

   /// called when a key is pressed
   bool HookCallback(UINT uVKcode, bool bKeyDown, bool bSysKey);

private:
   /// hardware key manager singleton
   static boost::scoped_ptr<CHardwareKeyManager> s_scpHardwareManager;

   /// registered hotkeys
   std::set<UINT> m_setRegisteredHotkeys;

   /// registered callback
   IHardwareKeyCallback* m_pCallback;

   /// keyboard hook handle
   HHOOK m_hHook;

   /// function pointer to SetWindowHookExW
   T_fnSetWindowsHookExW   m_fnSetWindowsHookExW;
   /// function pointer to CallNextHookEx
   T_fnCallNextHookEx      m_fnCallNextHookEx;
   /// function pointer to UnhookWindowsHookEx
   T_fnUnhookWindowsHookEx m_fnUnhookWindowsHookEx;
   /// function pointer to RegisterHotKey
   T_fnRegisterHotKey      m_fnRegisterHotKey;
   /// function pointer to UnregisterFunc1
   T_fnUnregisterFunc1     m_fnUnregisterFunc1;
};
