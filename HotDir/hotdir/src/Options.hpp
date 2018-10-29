//
// HotDir - Colored directory tool
// Copyright (C) 1999-2018 Michael Fink
//
/// \file Options.hpp Application options
//
#pragma once

#include <vector>
#include <string>

class Console;

/// \brief Application options
struct Options
{
public:
   /// ctor; initializes the object with default options
   Options()
      :m_showHelp(false),
      m_numColumns(4),
      m_sortAttribute(sortByExtension),
      m_sortReverse(false),
      m_lineByLine(true),
      m_showEmpty(false),
      m_recurseDirectories(false),
      m_treeMode(false)
   {
   }

   /// Parses command line
   void ParseCommandLine(const std::vector<std::string>& args, Console& console);

   /// Parses a single option
   void ParseOption(const std::string& arg, Console & console);

   /// Adds a path to the path list
   void AddPath(std::string path);

   /// Indicates if help should be shown
   bool m_showHelp;

   /// The number of file columns to show for directories
   unsigned int m_numColumns;

   /// Sort attribute
   enum SortAttribute
   {
      sortByName,       ///< Sort by name
      sortByExtension,  ///< Sort by file extension
      sortBySize,       ///< Sort by file size
      sortByDate,       ///< Sort by file date
      sortByTime,       ///< Sort by file time
   };

   /// Sort attribute for sorting file entries
   SortAttribute m_sortAttribute;

   /// Indicates that sort order is reversed
   bool m_sortReverse;

   /// Indicates if line-by-line mode should be used (files are sorted by line
   /// instead of by column).
   bool m_lineByLine;

   /// Indicates if empty directory entries should be shown
   bool m_showEmpty;

   /// Indicates if all sub directories of specified paths should also be listed
   bool m_recurseDirectories;

   /// Indicates if directory tree should be shown instead of directory file entries
   bool m_treeMode;

   /// List of path names to show
   std::vector<std::string> m_pathList;
};
