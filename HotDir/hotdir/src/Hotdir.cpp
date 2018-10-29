//
// HotDir - Colored directory tool
// Copyright (C) 1999-2018 Michael Fink
//
/// \file Hotdir.cpp HotDir application
//
#include "pch.h"
#include "hotdir.hpp"
#include "DirectoryViewer.hpp"
#include "StatisticsViewer.hpp"
#include "TreeViewer.hpp"
#include <set>
#include <io.h>

Hotdir::Hotdir()
   :m_console("HotDir")
{
   m_console.SetWaitAtFullPage(true, " [hit a key!]");
}

void Hotdir::ParseCommandLine(std::vector<std::string> args)
{
   m_options.ParseCommandLine(args, m_console);

   if (m_options.m_recurseDirectories)
   {
      RecurseSubDirectories(m_options.m_pathList);
   }

   std::string configFilename = args[0];
   size_t pos = configFilename.rfind(".exe");
   configFilename.replace(pos, pos + 4, ".cfg");

   m_configFile.Load(configFilename);
}

void Hotdir::Run()
{
   if (m_options.m_treeMode)
      m_console.Printf("^4Hot^7Tree & ^2Disk^7Usage v7.0.0^7\n");
   else
      m_console.Printf("^4Hot^7Dir v7.0.0\n");

   if (m_options.m_showHelp)
      ShowHelp();
   else if (m_options.m_treeMode)
      ShowTree();
   else
      ShowDirectory();
}

void Hotdir::ShowHelp()
{
   m_console.Printf("^2Help:^7\n"
      "Syntax: HD ^e[-options] ^9[drive:]^a[path]^c[files]^7\n"
      "Options:\n"
      " ^e-h -?  ^7  shows this help you are reading now\n"
      " ^e-c^fX    ^7  uses ^fX^7 columns to show the directory\n"
      " ^e-u     ^7  displays directory column-ordered instead of line-ordered\n"
      " ^e-e     ^7  shows directories with no files\n"
      " ^e-p     ^7  doesn't pause when the screen is full\n"
      " ^e-r     ^7  recursively shows all sub-directories\n"
      " ^e-s^7[^f-^7]^fX   ^7sorts the directory, where ^fX^7 is one of these:\n"
      "          ^fn^7=name, ^fe^7=extension, ^fs^7=size, ^fd^7=date and "
      "^ft^7=time\n"
      "          ^7the optional ^f-^7 reverses the sort order\n"
      " ^e-t     ^7  shows the tree and the disk usage instead of the dir\n"
      "\n(c) 1999-2018 Michael Fink\n"
      "^ahttps://github.com/vividos/OldStuff/^7 - ^9vividos@asamnet.de^7\n"
      "HotDir is distrubuted under the ^fBSD 2-clause License^7. See ^fLicense.md^7 for details.\n"
   );
}

void Hotdir::ShowDirectory()
{
   StatisticsViewer statsViewer(m_console);

   for (auto path : m_options.m_pathList)
   {
      DirectoryViewer viewer(m_console, path);
      viewer.Show(m_options, m_configFile);

      statsViewer.AddPath(path, viewer.GetFileList());
   }

   statsViewer.Show();
}

void Hotdir::ShowTree()
{
   for (auto path : m_options.m_pathList)
   {
      TreeViewer viewer(m_console, path);
      viewer.Show();
   }
}

void Hotdir::RecurseSubDirectories(std::vector<std::string>& pathList)
{
   for (size_t pathIndex = 0; pathIndex < pathList.size(); pathIndex++)
   {
      std::string path = pathList[pathIndex];

      _finddata_t findData = { 0 };
      intptr_t handle = _findfirst(path.c_str(), &findData);

      if (handle != -1)
      {
         size_t insertIndex = pathIndex + 1;

         do
         {
            // skip non-directories and names starting with '.'
            if ((findData.attrib & _A_SUBDIR) == 0 ||
               findData.name[0] == '.')
            {
               continue;
            }

            size_t pos = path.find_last_of('\\');
            std::string subPath = path.substr(0, pos + 1);
            subPath += findData.name;
            subPath += path.substr(pos);

            pathList.insert(pathList.begin() + insertIndex++, subPath);

         } while (_findnext(handle, &findData) == 0);

         _findclose(handle);
      }
   }
}

/// main function for Hotdir
int main(int argc, const char** argv)
{
   Hotdir hotdir;

   hotdir.ParseCommandLine(std::vector<std::string>(argv, argv + argc));
   hotdir.Run();

   return 0;
}
