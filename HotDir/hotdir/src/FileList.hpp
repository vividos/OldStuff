//
// HotDir - Colored directory tool
// Copyright (C) 1999-2018 Michael Fink
//
/// \file FileList.hpp List of all files to show
//
#pragma once

#include <string>
#include <vector>
#include <functional>
#include "Options.hpp"

/// \brief File list
/// \details Contains a list of files with various attributes that can also be
/// sorted.
class FileList
{
public:
   /// File entry for one file
   struct FileEntry
   {
      std::string m_name;        ///< file name
      unsigned long long m_size; ///< size in bytes
      __time64_t m_time;         ///< time of file creation
      unsigned int m_attributes; ///< file attributes

      /// ctor; creates an empty file entry
      FileEntry()
         :m_size(0),
         m_time(0),
         m_attributes(0)
      {
      }

      /// Returns extension, without dot
      std::string Extension() const
      {
         size_t pos = m_name.find_last_of('.');
         return pos == -1 ? std::string() : m_name.substr(pos + 1);
      }

      /// Returns time part of file creation date
      unsigned int Time() const
      {
         return static_cast<int>(m_time % (24 * 60 * 60));
      }
   };

   /// ctor; creates an empty files list
   FileList();

   /// Collects all files with the given file specification in the list
   void GetFiles(const std::string& fileSpec);

   /// Sorts files by given sort attribute and sort direction
   void SortFiles(Options::SortAttribute sortAttribute, bool sortReverse);

   /// Returns file list; const access
   const std::vector<FileEntry>& Files() const { return m_fileList; }

   /// Returns total number of bytes in files
   unsigned long long TotalFileSize() const { return m_totalFileSize; }

   /// Returns total number of bytes on disk
   unsigned long long TotalDiskSize() const { return m_totalDiskSize; }

private:
   /// Returns a sort comparison function for given sort attribute and sort direction
   std::function<bool(const FileList::FileEntry&, const FileList::FileEntry&)> GetSortFunction(
      Options::SortAttribute sortAttribute, bool sortReverse);

private:
   /// File list
   std::vector<FileEntry> m_fileList;

   /// Total number of bytes in files
   unsigned long long m_totalFileSize;

   /// Total number of bytes on disk
   unsigned long long m_totalDiskSize;
};
