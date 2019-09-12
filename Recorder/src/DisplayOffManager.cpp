//
// Recorder - a GPS logger app for Windows Mobile
// Copyright (C) 2006-2019 Michael Fink
//
/// \file DisplayOffManager.cpp Display off manager
//
#include "StdAfx.h"
#include "DisplayOffManager.hpp"
#include "VideoPowerManager.hpp"
#include "Logger.hpp"

// note: these are defined in <winuserm.h>
struct VKeyToNameMap
{
   UINT uiVKey;
   LPCTSTR pszName;
} g_aKeys[] =
{
#ifdef _WIN32_WCE
   { VK_APP1         , _T("App key 1") },
   { VK_APP2         , _T("App key 2") },
   { VK_APP3         , _T("App key 3") },
   { VK_APP4         , _T("App key 4") },
   { VK_APP5         , _T("App key 5") },
   { VK_APP6         , _T("App key 6") },

   { VK_OFF           , _T("Off") },

   { VK_TSOFT1       , _T("Soft key 1") },
   { VK_TSOFT2       , _T("Soft key 2") },
   { VK_TTALK        , _T("Talk") },
   { VK_TEND         , _T("End") },
   { VK_THOME        , _T("Home") },
   { VK_TBACK        , _T("Back") },
   { VK_TSTAR        , _T("Star (*)") },
   { VK_TPOUND       , _T("Pound (#)") },

   { VK_TACTION      , _T("Action") },
   { VK_TRECORD      , _T("Record") },
   { VK_TFLIP        , _T("Flip") },
   { VK_TPOWER       , _T("Power") },
   { VK_TVOLUMEUP    , _T("Volume Up") },
   { VK_TVOLUMEDOWN  , _T("Volume Down") },

   { VK_TUP          , _T("Up") },
   { VK_TDOWN        , _T("Down") },
   { VK_TLEFT        , _T("Left") },
   { VK_TRIGHT       , _T("Right") },
#else
   { VK_RETURN       , _T("Return") },
#endif
};

LPCTSTR g_pszDispOffMgr = _T("app.dispoffmgr");

CDisplayOffManager::CDisplayOffManager()
#ifdef _WIN32_WCE
   :m_uiAssignedKey(VK_TSOFT1),
#else
   : m_uiAssignedKey(VK_RETURN),
#endif
   m_bAssignKey(false),
   m_bSwitchedOff(false),
   m_bWaitForKeyUp(false),
   m_dwKeyPressedMinTimespan(4 * 1000),
   m_keyManager(CHardwareKeyManager::GetInstance())
{
   LOG_INFO(_T("display off manager ctor"), g_pszDispOffMgr);

   for (unsigned int i = 0; i < sizeof(g_aKeys) / sizeof(*g_aKeys); i++)
      m_mapVKeyToName[g_aKeys[i].uiVKey] = g_aKeys[i].pszName;

   m_keyManager.SetCallback(this);
}

CDisplayOffManager::~CDisplayOffManager()
{
   m_keyManager.Stop();

   LOG_INFO(_T("display off manager dtor"), g_pszDispOffMgr);
}

CString CDisplayOffManager::GetKeyName(UINT uiKey) const throw()
{
   std::map<UINT, LPCTSTR>::const_iterator iter = m_mapVKeyToName.find(uiKey);
   if (iter == m_mapVKeyToName.end())
   {
      CString cszText;
      cszText.Format(_T("%04x"), uiKey);
      return cszText;
   }
   else
      return CString(iter->second);
}

void CDisplayOffManager::AssignKey(T_fnOnHardwareKey fnOnHardwareKey)
{
   LOG_INFO(_T("assigning key..."), g_pszDispOffMgr);

   m_fnOnHardwareKey = fnOnHardwareKey;
   m_bAssignKey = true;
   m_keyManager.Start(ModuleHelper::GetModuleInstance(), GetActiveWindow());
}

void CDisplayOffManager::StopAssign()
{
   LOG_INFO(_T("stopping assigning"), g_pszDispOffMgr);

   m_keyManager.ResetHotkeys();
   m_keyManager.Stop();

   m_bAssignKey = false;
   m_fnOnHardwareKey = T_fnOnHardwareKey();
}

void CDisplayOffManager::SwitchOff(T_fnOnHardwareKey fnOnHardwareKey)
{
   ATLASSERT(m_uiAssignedKey != 0);
   ATLASSERT(m_bAssignKey == false);

   LOG_INFO(_T("switching off display"), g_pszDispOffMgr);

   m_fnOnHardwareKey = fnOnHardwareKey;

   m_keyManager.ResetHotkeys();
   // note: we don't set the hotkey here, since we want to get notifications from
   // other keys, too, so that we can switch off the screen when it's the wrong key
   //m_keyManager.AddHotkey(m_uiAssignedKey);

   m_keyManager.Start(ModuleHelper::GetModuleInstance(), GetActiveWindow());

   if (CVideoPowerManager::IsAvailDisplayOff())
   {
      m_spVideoPowerManager.reset(new CVideoPowerManager);
      m_spVideoPowerManager->SwitchOffDisplay(true);

      m_bSwitchedOff = true;
   }
}

void CDisplayOffManager::SwitchOn()
{
   LOG_INFO(_T("switching on display"), g_pszDispOffMgr);

   m_spVideoPowerManager.reset();

   m_keyManager.ResetHotkeys();
   m_keyManager.Stop();

   m_bSwitchedOff = true;
   m_fnOnHardwareKey = T_fnOnHardwareKey();
}

bool CDisplayOffManager::OnHardwareKey(UINT uVKCode, bool bKeyDown, bool bSysKey)
{
   {
      CString cszText;
      cszText.Format(_T("hardware key pressed, vk=%04x, %s, %s"), uVKCode,
         bKeyDown ? _T("down") : _T("up"),
         bSysKey ? _T("sys key") : _T("normal key"));
      LOG_INFO(cszText, g_pszDispOffMgr);
   }

   if (m_bAssignKey)
   {
      // while assigning a key
      m_uiAssignedKey = uVKCode;

      LOG_INFO(_T("assigned a new key"), g_pszDispOffMgr);

      m_bAssignKey = false;
   }
   else if (m_bSwitchedOff)
   {
      // key down event, and not yet waiting for key up?
      if (bKeyDown && !m_bWaitForKeyUp)
      {
         // check if it's our key
         if (uVKCode == m_uiAssignedKey)
         {
            // yes, wait for up key and note down ticks
            m_bWaitForKeyUp = true;
            m_dwKeyDownTime = GetTickCount();
         }
         else
         {
            // switch off screen again, since it wasn't our assigned key
            if (m_spVideoPowerManager.get() != NULL)
               m_spVideoPowerManager->SwitchOffDisplay(true);
         }
      }

      // key up event, and waiting for it?
      if (!bKeyDown && m_bWaitForKeyUp)
      {
         m_bWaitForKeyUp = false;

         // check if it's our key and if it occured after elapsed time
         DWORD dwCurrentTicks = GetTickCount();
         if (uVKCode == m_uiAssignedKey && dwCurrentTicks - m_dwKeyDownTime > m_dwKeyPressedMinTimespan)
         {
            // yes: switch back on screen
            SwitchOn();
            m_bSwitchedOff = false;
         }
         else
         {
            // not our key, or inside the timespan: switch back off
            if (m_spVideoPowerManager.get() != NULL)
               m_spVideoPowerManager->SwitchOffDisplay(true);
         }
      }
   }
   else
      return false; // not assigning a key and not switched off: do nothing

   // notify caller, in case more work has to be done (e.g. enabling buttons, etc.)
   if (m_fnOnHardwareKey)
      m_fnOnHardwareKey(m_uiAssignedKey);

   // tell hardware key manager that this key was handled and doesn't need further
   // processing
   return false;
}
