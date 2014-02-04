//
// ulib - a collection of useful classes
// Copyright (C) 2006-2014 Michael Fink
//
/// \file CommandLineParser.cpp command line parser
//

// includes
#include "stdafx.h"
#include <ulib/CommandLineParser.hpp>

CommandLineParser::CommandLineParser(const CString& cszText)
:m_cszCommandLine(cszText),
 m_iCurrentIndex(0)
{
}

CommandLineParser::CommandLineParser(int argc, TCHAR* argv[])
:m_iCurrentIndex(0)
{
   for (int i=0; i<argc; i++)
   {
      m_cszCommandLine += argv[i];
      if (i < argc-1)
         m_cszCommandLine += _T(" ");
   }
}

bool CommandLineParser::GetNext(CString& cszNextParam)
{
   if (m_cszCommandLine.IsEmpty() || m_iCurrentIndex >= m_cszCommandLine.GetLength())
      return false;

   // find out next stopper
   TCHAR chStopper = _T(' ');
   if (m_cszCommandLine[m_iCurrentIndex] == _T('\"'))
   {
      m_iCurrentIndex++;
      chStopper = _T('\"');
   }

   // search for stopper
   int iMax = m_cszCommandLine.Find(chStopper, m_iCurrentIndex);

   // extract next parameter and advance index
   if (iMax == -1)
   {
      cszNextParam = m_cszCommandLine.Mid(m_iCurrentIndex);
      m_iCurrentIndex = m_cszCommandLine.GetLength();
   }
   else
   {
      cszNextParam = m_cszCommandLine.Mid(m_iCurrentIndex, iMax - m_iCurrentIndex);
      m_iCurrentIndex = iMax+1;
   }

   // trim " stopper
   if (chStopper != _T(' '))
   {
#if (_ATL_VER < 0x0700)
      cszNextParam.TrimLeft(_T("\""));
      cszNextParam.TrimRight(_T("\""));
#else
      cszNextParam.Trim(_T("\""));
#endif
   }

   // eat space chars
   while (m_iCurrentIndex < m_cszCommandLine.GetLength() &&
         m_cszCommandLine[m_iCurrentIndex] == ' ')
      m_iCurrentIndex++;

   return !m_cszCommandLine.IsEmpty();
}
