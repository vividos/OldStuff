//
// HotDir - Colored directory tool
// Copyright (C) 1999-2018 Michael Fink
//
/// \file Options.cpp Application options
//
#include "pch.h"
#include "options.hpp"
#include "console.hpp"
#include <algorithm>
#include <direct.h>

void Options::ParseCommandLine(const std::vector<std::string>& args, Console& console)
{
   bool isFirst = true;

   for (auto arg : args)
   {
      // skip first; it's the executable name
      if (isFirst)
      {
         isFirst = false;
         continue;
      }

      if (arg.size() > 0 && (arg[0] == '-' || arg[0] == '/'))
      {
         ParseOption(arg, console);

      }
      else
      {
         AddPath(arg);
      }
   }

   if (m_pathList.empty())
   {
      AddPath("*.*");
   }
}

void Options::ParseOption(const std::string& arg, Console& console)
{
   switch (tolower(arg[1]))
   {
   case 'h':
   case '?':
      m_showHelp = true;
      break;

   case 'c':
      if (arg.size() > 1)
      {
         m_numColumns = static_cast<unsigned int>(arg[2] - '0');

         if (m_numColumns == 0 || m_numColumns > 6)
            m_numColumns = 4;
      }
      break;

   case 's':
      for (size_t i = 2; i < arg.size(); i++)
      {
         switch (arg[i])
         {
         case 'n': m_sortAttribute = sortByName; break;
         case 'e': m_sortAttribute = sortByExtension; break;
         case 's': m_sortAttribute = sortBySize; break;
         case 'd': m_sortAttribute = sortByDate; break;
         case 't': m_sortAttribute = sortByTime; break;
         case '-': m_sortReverse = true; break;
         default: break;
         }
      }
      break;

   case 'u':
      m_lineByLine = false;
      break;

   case 'e':
      m_showEmpty = true;
      break;

   case 'p':
      console.SetWaitAtFullPage(false, "");
      break;

   case 'r':
      m_recurseDirectories = true;
      m_treeMode = false;
      break;

   case 't':
      m_treeMode = true;
      m_recurseDirectories = false;
      break;

   default:
      console.Printf("option [%s] not recognized\n", arg.c_str());
   };
}

void Options::AddPath(std::string path)
{
   if (path.substr(0, 2) == "\\\\")
      return; // network path; do nothing

   std::replace(path.begin(), path.end(), '/', '\\');

   if (std::string::npos == path.find(':'))
   {
      path.insert(0, "x:");
      path[0] = static_cast<char>(_getdrive() - 1 + 'a');
   }

   if (path.size() == 2 && path[1] == ':')
   {
      path += '\\';
   }

   if (path.find('*') == std::string::npos)
   {
      if (path.substr(path.size() - 1, 1) == "\\")
         path += "*.*";
      else
      {
         if ((::GetFileAttributesA(path.c_str()) & FILE_ATTRIBUTE_DIRECTORY) != 0)
            path += "\\*.*";
      }
   }

   if (path.size() > 2 && path[2] != '\\' && path[1] == ':')
   {
      std::vector<char> buffer(MAX_PATH);
      _getdcwd(path[0] & 0x1f, buffer.data(), static_cast<int>(buffer.size()));
      buffer.push_back(0);

      std::string currentPath = buffer.data() + 2;
      currentPath += '\\';

      path.insert(2, currentPath);
   }

   m_pathList.push_back(path);
}
