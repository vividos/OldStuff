#pragma once

// includes
#include <ulib/wince/HardwareKeyManager.hpp>
#include <boost/function.hpp>
#include <boost/scoped_ptr.hpp>
#include <map>

/// function type used when hardware key was pressed
#if defined(_WIN32_WCE) && _MSC_VER < 1300
typedef boost::function1<void, UINT> T_fnOnHardwareKey;
#else
typedef boost::function<void(UINT)> T_fnOnHardwareKey;
#endif

class CVideoPowerManager;

/// display off manager
/*! The display off manager lets users assign a hardware key that is later used to
    switch on the display when switched off. The AssignKey() and SwitchOff() methods
    accept an optional function pointer that is called when either the hardware key
    was pressed that is assigned or when the display is switched back on.

    Usage is normally to let the user assign a key by pressing a button; call
    AssignKey() for that. If the user wants to cancel the assigning, call
    CancelAssign(). When the user wants to switch off the display, call SwitchOff().
    The SwitchOn() is called automatically when the assigned key is pressed. You can
    call the function at program exit, to ensure switching back on the display. The
    passed callback functions serve to let the user interface enable or disable
    buttons or to update UI elements.
*/
class CDisplayOffManager: public IHardwareKeyCallback
{
public:
   /// ctor
   CDisplayOffManager();
   /// dtor
   ~CDisplayOffManager();

   /// sets minimum timespan an assigned key has to be pressed to switch on screen; in ms; default is 4000
   void SetKeyPressedMinTimespan(DWORD dwKeyPressedMinTimespan) throw()
   {
      m_dwKeyPressedMinTimespan = dwKeyPressedMinTimespan;
   }

   /// returns assigned key (or 0 if none was assigned)
   UINT AssignedKey() const throw() { return m_uiAssignedKey; }

   /// returns key name from virtual key identifier (only pocketpc related keys)
   CString GetKeyName(UINT uiKey) const throw();

   /// starts assigning a key; next pressed key will become the assigned key
   void AssignKey(T_fnOnHardwareKey fnOnHardwareKey = T_fnOnHardwareKey());
   /// stops assigning a key
   void StopAssign();

   /// switches off display
   void SwitchOff(T_fnOnHardwareKey fnOnHardwareKey);
   /// switches back on display (normally called by manager itself)
   void SwitchOn();

private:
   // IHardwareKeyCallback virtual functions

   /// called when hardware key press occurs
   virtual bool OnHardwareKey(UINT uVKCode, bool bKeyDown, bool bSysKey);

private:
   /// currently assigned key
   UINT m_uiAssignedKey;

   /// indicates if we currently assign a key
   bool m_bAssignKey;

   /// indicates if we currently switched off display
   bool m_bSwitchedOff;

   /// indicates if waiting for a key up event
   bool m_bWaitForKeyUp;

   /// minimum timespan an assigned key has to be pressed to switch on screen; in ms
   DWORD m_dwKeyPressedMinTimespan;

   /// time the key down occured
   DWORD m_dwKeyDownTime;

   /// hardware key manager (singleton ref)
   CHardwareKeyManager& m_keyManager;

   /// video power manager
   boost::scoped_ptr<CVideoPowerManager> m_spVideoPowerManager;

   /// callback for hardware key
   T_fnOnHardwareKey m_fnOnHardwareKey;

   /// map with virtual keys to names
   std::map<UINT, LPCTSTR> m_mapVKeyToName;
};
