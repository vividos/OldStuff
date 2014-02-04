//
// ulib - a collection of useful classes
// Copyright (C) 2006-2014 Michael Fink
//
/// \file PrivilegeManager.hpp Privilege manager
//
#pragma once

#ifdef _WIN32_WCE
#  error PrivilegeManager cannot be used under Windows CE!
#endif

/// \brief Win32 classes
namespace Win32
{

/// \brief privilege manager
/// Get and set win32 process privileges. For setting priviliges,
/// a token handle with TOKEN_ADJUST_PRIVILEGES access is needed.
/// For getting priviliges, TOKEN_QUERY access is needed.
/// Open the token using OpenProcessToken(GetCurrentProcess(), access, &hToken);
class PrivilegeManager
{
public:
   /// ctor; takes token
   /// \param hToken access token handle
   PrivilegeManager(HANDLE hToken):m_hToken(hToken){}

   /// returns if privilege is currently enabled in given token
   bool Get(LPCTSTR pszPrivilegeName) const;

   /// returns display name string for given privilege
   CString GetDisplayName(LPCTSTR pszPrivilegeName) const;

   /// sets privilege
   bool Set(LPCTSTR pszPrivilege, bool bEnablePrivilege);

private:
   /// process token
   HANDLE m_hToken;
};

} // namespace Win32
