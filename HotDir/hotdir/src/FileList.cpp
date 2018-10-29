//
// HotDir - Colored directory tool
// Copyright (C) 1999-2018 Michael Fink
//
/// \file FileList.cpp List of all files to show
//
#include "pch.h"
#include "FileList.hpp"
#include <io.h>
#include <direct.h>
#include <algorithm>

FileList::FileList()
   :m_totalFileSize(0),
   m_totalDiskSize(0)
{
}

void FileList::GetFiles(const std::string& fileSpec)
{
   unsigned long clustersize = 512;

   if (fileSpec.size() > 1 &&
      fileSpec[1] == ':')
   {
      unsigned int drive = fileSpec[0] & 0x1f;
      if (drive > 0)
      {
         _diskfree_t diskfree = { 0 };
         unsigned int ret = _getdiskfree(drive, &diskfree);

         clustersize = ret != 0 ? 4096 :
            diskfree.bytes_per_sector * diskfree.sectors_per_cluster;
      }
   }

   _finddata_t findData = { 0 };
   intptr_t handle = _findfirst(fileSpec.c_str(), &findData);

   if (handle != -1)
   {
      do
      {
         // skip directories
         if ((findData.attrib & _A_SUBDIR) != 0)
            continue;

         FileEntry entry;
         entry.m_name = findData.name;
         entry.m_attributes = findData.attrib;
         entry.m_time = findData.time_write;
         entry.m_size = findData.size;

         m_fileList.push_back(entry);

         m_totalFileSize += findData.size;
         m_totalDiskSize += findData.size;

         if (findData.size % clustersize != 0)
            m_totalDiskSize += clustersize - (findData.size % clustersize);

      } while (_findnext(handle, &findData) == 0);

      _findclose(handle);
   }
}

void FileList::SortFiles(Options::SortAttribute sortAttribute, bool sortReverse)
{
   std::function<bool(const FileEntry&, const FileEntry&)> compareFunc =
      GetSortFunction(sortAttribute, sortReverse);

   if (compareFunc != nullptr)
      std::stable_sort(m_fileList.begin(), m_fileList.end(), compareFunc);
}

std::function<bool(const FileList::FileEntry&, const FileList::FileEntry&)> FileList::GetSortFunction(
   Options::SortAttribute sortAttribute, bool sortReverse)
{
   std::function<bool(const FileEntry&, const FileEntry&)> compareFunc;

   switch (sortAttribute)
   {
   case Options::sortByName:
      compareFunc = [sortReverse](const FileEntry& entry1, const FileEntry& entry2)
      {
         return (entry1.m_name < entry2.m_name) ^ sortReverse;
      };
      break;

   case Options::sortByExtension:
      compareFunc = [sortReverse](const FileEntry& entry1, const FileEntry& entry2)
      {
         return (entry1.Extension() < entry2.Extension()) ^ sortReverse;
      };
      break;

   case Options::sortBySize:
      compareFunc = [sortReverse](const FileEntry& entry1, const FileEntry& entry2)
      {
         return (entry1.m_size < entry2.m_size) ^ sortReverse;
      };
      break;

   case Options::sortByDate:
      compareFunc = [sortReverse](const FileEntry& entry1, const FileEntry& entry2)
      {
         return (entry1.m_time < entry2.m_time) ^ sortReverse;
      };
      break;

   case Options::sortByTime:
      compareFunc = [sortReverse](const FileEntry& entry1, const FileEntry& entry2)
      {
         return (entry1.Time() < entry2.Time()) ^ sortReverse;
      };
      break;

   default:
      break;
   }

   return compareFunc;
}
