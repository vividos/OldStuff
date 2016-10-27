//
// MrCrash - crash info reporting tool
// Copyright (C) 2005-2014 Michael Fink
//
/// \file AppOptions.cpp Application options
//

// includes
#include "StdAfx.h"
#include "AppOptions.hpp"
#include <ulib/ProgramOptions.hpp>
#include <functional>

AppOptions::AppOptions(LPCTSTR pszCommandLine)
:m_enStartMode(smInteractive),
 m_dwAttachProcessId(0)
{
   ProgramOptions opt;

   ProgramOptions::T_fnOptionHandlerNoArg fnOnParamInteractive =
      std::bind(&AppOptions::OnParamInteractive, this);
   opt.RegisterOption(_T("i"), _T("interactive"), _T("Interactive mode (default)"), fnOnParamInteractive);

   ProgramOptions::T_fnOptionHandlerSingleArg fnOnParamAttach =
      std::bind(&AppOptions::OnParamAttach, this, std::placeholders::_1);

   opt.RegisterOption(_T("a"), _T("attach"),
      _T("Attach to process with pid given in next parameter (pid may be decimal or hex, when prefixed with 0x)"),
      fnOnParamAttach);

   ProgramOptions::T_fnOptionHandlerSingleArg fnOnParamStart =
      std::bind(&AppOptions::OnParamStart, this, std::placeholders::_1);
   opt.RegisterOption(_T("s"), _T("start"),
      _T("Start application given in next parameter"),
      fnOnParamStart);

   ProgramOptions::T_fnOptionHandlerSingleArg fnOnParamCommandLine =
      std::bind(&AppOptions::OnParamCommandLine, this, std::placeholders::_1);
   opt.RegisterOption(_T("c"), _T("command"),
      _T("Uses command line parameters given in next parameter, when /s is used"),
      fnOnParamCommandLine);

   if (_tcslen(pszCommandLine) > 0)
      opt.Parse(pszCommandLine);
}

bool AppOptions::OnParamInteractive()
{
   m_enStartMode = smInteractive;
   return true;
}

bool AppOptions::OnParamAttach(const CString& cszParamProcessId)
{
   if (m_enStartMode == smStartProcess)
      _tprintf(_T("already selected \"start process\" as operating mode."));
   else
   {
      m_enStartMode = smAttachProcess;

      CString cszProcessId(cszParamProcessId);
      cszProcessId.MakeLower();
      if (cszProcessId.Find(_T("0x")) == 0)
         m_dwAttachProcessId = _tcstoul(cszProcessId, NULL, 16);
      else
         m_dwAttachProcessId = _tcstoul(cszProcessId, NULL, 10);

      _tprintf(_T("attaching to process id 0x%08x (%u)\n"), m_dwAttachProcessId, m_dwAttachProcessId);
   }
   return true;
}

bool AppOptions::OnParamStart(const CString& cszStartApp)
{
   m_enStartMode = smStartProcess;
   m_cszStartApp = cszStartApp;

   _tprintf(_T("starting application: [%s]\n"), m_cszStartApp.GetString());
   return true;
}

bool AppOptions::OnParamCommandLine(const CString& cszCmdLine)
{
   m_cszCmdLine = cszCmdLine;

   if (m_enStartMode != smStartProcess)
      _tprintf(_T("you have to specify an application first, using option /s\n"));
   else
   {
      _tprintf(_T("using command line: [%s]\n"), m_cszCmdLine.GetString());
   }
   return true;
}
