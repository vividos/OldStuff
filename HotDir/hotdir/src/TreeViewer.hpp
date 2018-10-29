//
// HotDir - Colored directory tool
// Copyright (C) 1999-2018 Michael Fink
//
/// \file TreeViewer.hpp Directory tree viewer
//
#pragma once

#include <string>
#include <vector>

class Console;

/// \brief Directory tree viewer
/// \details Shows a path as directory tree.
class TreeViewer
{
public:
   /// ctor
   TreeViewer(Console& console, const std::string& path);

   /// Shows directory tree
   void Show();

private:
   /// Collects tree entries for given path and recurse depth
   unsigned long long RecursiveGetTreeEntries(const std::string& path, unsigned int depth);

   /// Checks if there are tree entries with the same or higher levels,
   /// starting from the given index
   bool FindNextLevel(unsigned int nr, size_t from);

private:
   /// Console to use for output
   Console& m_console;

   /// Path of directory to traverse
   std::string m_path;

   /// Tree entry
   struct TreeEntry
   {
      /// Name of directory
      std::string m_name;

      /// Level of tree depth
      unsigned int m_level;

      /// Size of all files in this directory
      unsigned long long m_size;

      /// ctor
      TreeEntry()
         :m_level(0),
         m_size(0)
      {
      }
   };

   /// List of tree entries
   std::vector<TreeEntry> m_treeEntryList;

   /// Total size of all files in the sub directories, in bytes
   unsigned long long m_totalSize;
};
