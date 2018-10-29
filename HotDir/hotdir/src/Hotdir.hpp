//
// HotDir - Colored directory tool
// Copyright (C) 1999-2018 Michael Fink
//
/// \file Hotdir.hpp HotDir application
//
#pragma once

#include <vector>
#include <string>
#include "Console.hpp"
#include "Options.hpp"
#include "ConfigFile.hpp"

/// \brief HotDir application
/// \details Parses command line parameters and shows help, directory entries
/// or directory tree.
class Hotdir
{
public:
   /// ctor
   Hotdir();

   /// Parses command line arguments
   void ParseCommandLine(std::vector<std::string> args);

   /// Runs the application
   void Run();

private:
   /// Shows help for the application
   void ShowHelp();

   /// Shows directory entries
   void ShowDirectory();

   /// Shows infos fir a single drive letter
   void ShowDriveInfos(char driveLetter);

   /// Shows directory tree
   void ShowTree();

   /// Recursively add all sub directories to the path list
   void RecurseSubDirectories(std::vector<std::string>& pathList);

private:
   /// Console to output text
   Console m_console;

   /// Application options
   Options m_options;

   /// Configuration file
   ConfigFile m_configFile;
};
