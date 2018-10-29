//
// HotDir - Colored directory tool
// Copyright (C) 1999-2018 Michael Fink
//
/// \file StatisticsViewer.cpp Statistics viewer
//
#include "pch.h"
#include "StatisticsViewer.hpp"
#include "Console.hpp"
#include "FileList.hpp"
#include "FormatHelper.hpp"
#include <io.h>

StatisticsViewer::StatisticsViewer(Console& console)
   :m_console(console),
   m_pathCount(0),
   m_fileCount(0),
   m_totalFileSize(0ULL),
   m_totalDiskSize(0ULL)
{
}

void StatisticsViewer::AddPath(const std::string& path, const FileList& fileList)
{
   if (path.size() > 1 &&
      path[1] == ':')
   {
      m_driveLetters.insert(static_cast<char>(toupper(path[0])));
   }

   m_pathCount++;
   m_fileCount += fileList.Files().size();

   m_totalFileSize += fileList.TotalFileSize();
   m_totalDiskSize += fileList.TotalDiskSize();
}

void StatisticsViewer::Show() const
{
   if (m_pathCount == 0)
      m_console.Printf("^7sorry, no output.\n");

   if (m_pathCount > 1)
      ShowAllPathInfos();

   for (char driveLetter : m_driveLetters)
      ShowDriveInfos(driveLetter);
}

void StatisticsViewer::ShowAllPathInfos() const
{
   m_console.Printf(
      "^4%u^2 paths, ^4%lu^2 files, using ^4%s^2 bytes, needing ^4%s^2 bytes.^7\n",
      m_pathCount,
      m_fileCount,
      FormatHelper::FormatDecimal(m_totalFileSize).c_str(),
      FormatHelper::FormatDecimal(m_totalDiskSize).c_str());
}

void StatisticsViewer::ShowDriveInfos(char driveLetter) const
{
   unsigned int drive = driveLetter & 0x1f;

   _diskfree_t diskfree = { 0 };
   unsigned int ret = _getdiskfree(drive, &diskfree);

   if (ret != 0)
   {
      m_console.Printf("^cError: ^aCan't get infos for drive ^c%c:", driveLetter);
      return;
   }

   unsigned long long clusterSize = diskfree.sectors_per_cluster * diskfree.bytes_per_sector;
   unsigned long long freeBytes = diskfree.avail_clusters * clusterSize;
   unsigned long long totalBytes = diskfree.total_clusters * clusterSize;

   m_console.Printf(
      "^adrive ^c%c:^a info: ^c%s^a bytes of ^c%s^a bytes available.^7\n",
      driveLetter,
      FormatHelper::FormatDecimal(freeBytes).c_str(),
      FormatHelper::FormatDecimal(totalBytes).c_str());

   unsigned int rate = static_cast<unsigned int>((double)diskfree.avail_clusters / diskfree.total_clusters * 100 * 100.0);

   std::string label = GetVolumeLabel(driveLetter);
   if (label.empty())
      label = "^anone";

   m_console.Printf(
      "         ^c%u.%2.2u%%^a free; Label: ^e%.11s^7\n",
      rate / 100,
      rate % 100,
      label.c_str());
}

std::string StatisticsViewer::GetVolumeLabel(char driveLetter)
{
   std::string rootPath(1, driveLetter);
   rootPath += ":\\";

   std::vector<char> labelBuffer(MAX_PATH);

   DWORD maximumComponentLength = 0;
   DWORD fileSystemFlags = 0;

   bool ret = GetVolumeInformationA(
      rootPath.c_str(),
      labelBuffer.data(),
      static_cast<DWORD>(labelBuffer.size()),
      nullptr,
      &maximumComponentLength,
      &fileSystemFlags,
      nullptr,
      0);

   std::string label;

   if (ret)
      label = labelBuffer.data();

   return label;
}
