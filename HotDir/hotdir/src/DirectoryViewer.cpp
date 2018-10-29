//
// HotDir - Colored directory tool
// Copyright (C) 1999-2018 Michael Fink
//
/// \file DirectoryViewer.cpp Directory viewer
//
#include "pch.h"
#include "DirectoryViewer.hpp"
#include "Console.hpp"
#include "Options.hpp"
#include "ConfigFile.hpp"
#include "FileList.hpp"
#include "FormatHelper.hpp"
#include <io.h>
#include <time.h>
#include <array>

#undef min

DirectoryViewer::DirectoryViewer(Console& console, const std::string& path)
   :m_console(console),
   m_path(path)
{
}

void DirectoryViewer::Show(const Options& options, const ConfigFile& configFile)
{
   m_fileList.GetFiles(m_path);

   if (!options.m_showEmpty && m_fileList.Files().empty())
      return;

   m_fileList.SortFiles(options.m_sortAttribute, options.m_sortReverse);

   m_console.Printf("^fPath: ^e%s\n", m_path.c_str());

   if (m_fileList.Files().empty())
   {
      m_console.Printf("no files found.\n");
      return;
   }

   if (options.m_numColumns == 1 ||
      m_fileList.Files().size() < options.m_numColumns)
      ShowLineOrdered(configFile, 1);
   else
   {
      if (options.m_lineByLine)
         ShowLineOrdered(configFile, options.m_numColumns);
      else
         ShowColumnOrdered(configFile, options.m_numColumns);
   };

   ShowFileInfos();
}

void DirectoryViewer::ShowLineOrdered(const ConfigFile& configFile, unsigned int numColumns)
{
   size_t maxIndex = m_fileList.Files().size();
   for (size_t index = 0; index < maxIndex; index++)
   {
      unsigned int columnNumber = index % numColumns;

      ShowEntry(m_fileList.Files()[index], configFile, columnNumber, numColumns);
   }

   if ((maxIndex % numColumns) != 0)
      m_console.Printf("^7\n");
}

void DirectoryViewer::ShowColumnOrdered(const ConfigFile& configFile, unsigned int numColumns)
{
   size_t numFiles = m_fileList.Files().size();

   size_t columnStep = numFiles / numColumns;
   if (numFiles % numColumns != 0)
      columnStep++;

   std::vector<std::vector<FileList::FileEntry>::const_iterator> fileIterators(numColumns);
   for (unsigned columnIndex = 0; columnIndex < numColumns; columnIndex++)
   {
      auto iter = m_fileList.Files().begin();
      std::advance(iter, std::min(columnStep * columnIndex, numFiles));
      fileIterators[columnIndex] = iter;
   }

   for (size_t lineNumber = 0; lineNumber < columnStep; lineNumber++)
   {
      bool endIteratorAppeared = false;
      for (size_t columnNumber = 0; columnNumber < numColumns; columnNumber++)
      {
         auto& iter = fileIterators[columnNumber];

         if (iter == m_fileList.Files().end())
         {
            if (!endIteratorAppeared)
            {
               m_console.Printf("\n");
               endIteratorAppeared = true;
            }

            continue;
         }

         ShowEntry(*iter, configFile, static_cast<unsigned int>(columnNumber), numColumns);

         iter++;
      }
   }

   if (numFiles % numColumns != 0)
      m_console.Printf("^7\n");
}

void DirectoryViewer::ShowEntry(const FileList::FileEntry& fileEntry, const ConfigFile& configFile, unsigned int columnNumber, unsigned int numColumns)
{
   unsigned char color = static_cast<unsigned char>(configFile.GetColorByExtension(fileEntry.Extension()));
   color += color < 10 ? '0' : 'a' - 10;

   unsigned int columnWidth = m_console.Width() / numColumns;

   if (columnWidth > 89)
   {
      m_console.Printf("^4³"); // 1
      ShowDate(fileEntry.m_time, false); // 11
      ShowTime(fileEntry.m_time, false); // 9
      ShowAttributes(fileEntry.m_attributes); // 6
      m_console.Printf(" "); // 1
      ShowSize(fileEntry.m_size, fileEntry.m_attributes, false); // 16

      int remainingSpace = columnWidth - 1 - 11 - 9 - 6 - 1 - 16 - 1;
      m_console.Printf("^%c%-*s^7", color, remainingSpace, fileEntry.m_name.c_str());
   }
   else if (columnWidth > 80)
   {
      m_console.Printf("^4³"); // 1
      ShowDate(fileEntry.m_time, false); // 11
      ShowTime(fileEntry.m_time, false); // 9
      ShowAttributes(fileEntry.m_attributes); // 6
      m_console.Printf(" "); // 1
      ShowSize(fileEntry.m_size, fileEntry.m_attributes, true); // 7
      m_console.Printf("^%c%-44.44s^7", color, fileEntry.m_name.c_str()); // 44
   }
   else if (columnWidth > 40)
   {
      m_console.Printf("^4³"); // 1
      ShowDate(fileEntry.m_time, true); // 9
      ShowTime(fileEntry.m_time, true); // 6
      ShowSize(fileEntry.m_size, fileEntry.m_attributes, true); // 7
      m_console.Printf("^%c%-17.17s^7", color, fileEntry.m_name.c_str()); // 17
   }
   else if (columnWidth > 26)
   {
      m_console.Printf("^4³"); // 1
      ShowAttributes(fileEntry.m_attributes); // 6
      ShowSize(fileEntry.m_size, fileEntry.m_attributes, true); // 7
      m_console.Printf("^%c%-12.12s", color, fileEntry.m_name.c_str()); // 12
      if (columnNumber + 1 == numColumns)
         m_console.Printf("^4³"); // 1
   }
   else if (columnWidth > 20)
   {
      m_console.Printf("^4³"); // 1
      ShowSize(fileEntry.m_size, fileEntry.m_attributes, true); // 7
      m_console.Printf("^%c%-12.12s", color, fileEntry.m_name.c_str()); // 12
   }
   else if (columnWidth > 16)
   {
      m_console.Printf("^4³^%c%-15.15s", color, fileEntry.m_name.c_str());
   }
   else if (columnWidth > 13)
   {
      m_console.Printf("^4³^%c%-12.12s^7", color, fileEntry.m_name.c_str());
      if (columnNumber + 1 == numColumns)
         m_console.Printf("^4³");
   }

   if (columnNumber + 1 == numColumns)
      m_console.Printf("\n");
}

/// \details outputs 7 characters in short format, and 16 characters in long format
void DirectoryViewer::ShowSize(unsigned long long size, unsigned int attributes, bool shortFormat)
{
   if ((attributes & _A_SUBDIR) != 0)
   {
      m_console.Printf(shortFormat ? "^d <dir> " : "^d <directory>  ");
      return;
   }

   if (!shortFormat)
   {
      std::string text = FormatHelper::FormatDecimal(size);
      m_console.Printf("^e%15.15s ", text.c_str());
      return;
   }

   if (size < 1000L)
   {
      m_console.Printf("^e% 6i ", static_cast<int>(size));
      return;
   }

   if (size < 1000L * 1000L)
   {
      m_console.Printf("^d% 5iK ", static_cast<int>(size / 1000L));
      return;
   }

   if (size < 100L * 1000L * 1000L)
   {
      m_console.Printf("^c% 5iM ", static_cast<int>(size / 1000L / 1000L));
      return;
   }
}

/// \details outputs 9 characters in short format, and 11 characters in long format
void DirectoryViewer::ShowDate(__time64_t time, bool shortFormat)
{
   tm tmtime;
   localtime_s(&tmtime, &time);

   char buffer[20];
   strftime(buffer, sizeof(buffer), shortFormat ? "%y-%m-%d" : "%Y-%m-%d", &tmtime);

   m_console.Printf("^a%s ", buffer);
}

/// \details outputs 6 characters in short format, and 9 characters in long format
void DirectoryViewer::ShowTime(__time64_t time, bool shortFormat)
{
   tm tmtime;
   localtime_s(&tmtime, &time);

   char buffer[20];
   strftime(buffer, sizeof(buffer), shortFormat ? "%H:%M" : "%H:%M:%S", &tmtime);

   m_console.Printf("^9%s ", buffer);
}

/// \details outputs 6 characters
void DirectoryViewer::ShowAttributes(unsigned int attributes)
{
   std::array<char, 6> buffer = { '-', '-', '-', '-', '-', 0 }; // 5 attributes

   if ((attributes & _A_SUBDIR) != 0)
      buffer[0] = 'd';
   if ((attributes & _A_RDONLY) != 0)
      buffer[1] = 'r';
   if ((attributes & _A_HIDDEN) != 0)
      buffer[2] = 'h';
   if ((attributes & _A_SYSTEM) != 0)
      buffer[3] = 's';
   if ((attributes & _A_ARCH) != 0)
      buffer[4] = 'a';

   m_console.Printf("^7%s ", buffer.data());
}

void DirectoryViewer::ShowFileInfos() const
{
   unsigned long long totalDiskSize = m_fileList.TotalDiskSize();
   unsigned long long totalFileSize = m_fileList.TotalFileSize();
   unsigned int ratePercent = 0;
   if (totalDiskSize != 0)
      ratePercent = static_cast<unsigned int>((double)totalFileSize / totalDiskSize * 100.0);

   m_console.Printf(
      "^c%u^a files, using ^c%s^a bytes, needing ^c%s^a bytes (^c%u%%^a).^7\n",
      m_fileList.Files().size(),
      FormatHelper::FormatDecimal(totalFileSize).c_str(),
      FormatHelper::FormatDecimal(totalDiskSize).c_str(),
      ratePercent);
}
