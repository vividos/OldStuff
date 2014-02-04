//
// ulib - a collection of useful classes
// Copyright (C) 2006-2014 Michael Fink
//
/// \file CommandLineParser.hpp command line parser
//
#pragma once

// includes
#include <ulib/config/AutoLink.hpp>

/// \brief command line parser
/// \details parses command lines; supports double-quoted parameters
class ULIB_DECLSPEC CommandLineParser
{
public:
   /// parses single long command line string
   CommandLineParser(const CString& cszText = GetCommandLine());

   /// parses _tmain parameter
   CommandLineParser(int argc, TCHAR* argv[]);

   /// returns next parameter
   bool GetNext(CString& cszNextParam);

private:
   /// command line
   CString m_cszCommandLine;

   /// current index into line
   int m_iCurrentIndex;
};
