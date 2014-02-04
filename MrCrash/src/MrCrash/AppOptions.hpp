//
// MrCrash - crash info reporting tool
// Copyright (C) 2005-2014 Michael Fink
//
/// \file AppOptions.hpp Application options
//
#pragma once

/// start mode
enum T_enStartMode
{
   smStartProcess,   ///< starts process
   smAttachProcess,  ///< attaches to running process
   smInteractive,    ///< interactive mode
};

/// application options
class AppOptions
{
public:
   AppOptions(LPCTSTR pszCommandLine);

   T_enStartMode StartMode() const throw() { return m_enStartMode; }

   DWORD AttachProcessId() const throw() { return m_dwAttachProcessId; }
   CString StartApp() const throw() { return m_cszStartApp; }
   CString CmdLine() const throw() { return m_cszCmdLine; }

private:
   bool OnParamInteractive();
   bool OnParamAttach(const CString& cszProcessId);
   bool OnParamStart(const CString& cszStartApp);
   bool OnParamCommandLine(const CString& cszCmdLine);

private:
   T_enStartMode m_enStartMode;

   DWORD m_dwAttachProcessId;
   CString m_cszStartApp;
   CString m_cszCmdLine;
};
