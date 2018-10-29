//
// HotDir - Colored directory tool
// Copyright (C) 1999-2018 Michael Fink
//
/// \file DirectoryViewer.hpp Directory viewer
//
#pragma once

#include <string>
#include "FileList.hpp"

class Console;
struct Options;
class ConfigFile;

/// \brief Directory viewer
/// \details Shows all files in a given path, by displaying them in multiple
/// colors. Depending on the number of columns, different amounts of
/// information of a file can be displayed. The files are colored by extension
/// and are sorted by the specified sort order from the Options.
/// Line-ordered means: The files are displayed line-by-line, with the first
/// files in the lines above and later files further below.
/// Column-ordered means: The files are displayed ordered in columns like in
/// newspaper articles.
class DirectoryViewer
{
public:
   /// ctor
   DirectoryViewer(Console& console, const std::string& path);

   /// Shows directory entries
   void Show(const Options& options, const ConfigFile& configFile);

   /// Returns file list that was filled and displayed in Show()
   const FileList& GetFileList() const { return m_fileList; }

private:
   /// Displays all files in line-ordered mode
   void ShowLineOrdered(const ConfigFile& configFile, unsigned int numColumns);

   /// Displays all files in column-ordered mode
   void ShowColumnOrdered(const ConfigFile& configFile, unsigned int numColumns);

   /// Shows a single file entry, based on number of columns
   void ShowEntry(const FileList::FileEntry& fileEntry, const ConfigFile& configFile, unsigned int columnNumber, unsigned int numColumns);

   /// Shows size of file entry
   void ShowSize(unsigned long long size, unsigned int attributes, bool shortFormat);

   /// Shows date of file entry
   void ShowDate(__time64_t time, bool shortFormat);

   /// Shows time of file entry
   void ShowTime(__time64_t time, bool shortFormat);

   /// Shows file attributes
   void ShowAttributes(unsigned int attributes);

   /// Shows file infos for the given file list
   void ShowFileInfos() const;

private:
   /// Console to use for output
   Console& m_console;

   /// path to print
   std::string m_path;

   /// File list collected in Show()
   FileList m_fileList;
};
