//
// HotDir - Colored directory tool
// Copyright (C) 1999-2018 Michael Fink
//
/// \file StatisticsViewer.hpp Viewer for statistics
//
#pragma once

#include <string>
#include <set>

class Console;
class FileList;

/// \brief Statistics viewer
/// \details Shows statistics for all file lists that were shown. For all
/// directories listed, number of files and total number of file size is
/// shown. For all drives that were visited, drive statistics like free and
/// total storage memory is shown.
class StatisticsViewer
{
public:
   /// ctor
   StatisticsViewer(Console& console);

   /// Adds path and file list to statistics
   void AddPath(const std::string& path, const FileList& fileList);

   /// Shows statistics
   void Show() const;

private:
   /// Shows file statistics for all shown directories
   void ShowAllPathInfos() const;

   /// Shows drive infos for given drive letter
   void ShowDriveInfos(char driveLetter) const;

   /// Finds volume label for given drive letter
   static std::string GetVolumeLabel(char driveLetter);

private:
   /// Console to use for output
   Console& m_console;

   /// Set of all uppercase drive letters used
   std::set<char> m_driveLetters;

   /// Number of paths shown
   size_t m_pathCount;

   /// Number of files shown
   size_t m_fileCount;

   /// Total file size for all files shown
   unsigned long long m_totalFileSize;

   /// Total disk size for all files shown
   unsigned long long m_totalDiskSize;
};
