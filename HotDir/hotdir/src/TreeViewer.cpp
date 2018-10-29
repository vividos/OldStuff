//
// HotDir - Colored directory tool
// Copyright (C) 1999-2018 Michael Fink
//
/// \file TreeViewer.cpp Directory tree viewer
//
#include "pch.h"
#include "TreeViewer.hpp"
#include "Console.hpp"
#include "FormatHelper.hpp"
#include <io.h>

TreeViewer::TreeViewer(Console& console, const std::string& path)
   :m_console(console),
   m_path(path)
{
   m_totalSize = RecursiveGetTreeEntries(path, 0);
}

void TreeViewer::Show()
{
   m_console.Printf("^fPath: ^e%s\n", m_path.c_str());

   for (size_t entryIndex = 0; entryIndex < m_treeEntryList.size(); entryIndex++)
   {
      auto entry = m_treeEntryList[entryIndex];

      m_console.Printf("^9");
      unsigned int level;
      for (level = 0; level < entry.m_level; level++)
         m_console.Printf("%c", FindNextLevel(level, entryIndex) ? '³' : ' ');

      m_console.Printf("%c^7%s", FindNextLevel(level, entryIndex) ? 'Ã' : 'À', entry.m_name.c_str());

      size_t length = entry.m_name.size() + level;

      m_console.Printf("%*c", 16 - (length % 16), ' ');

      m_console.Printf("^e %16s^7\n", FormatHelper::FormatDecimal(entry.m_size).c_str());
   };

   m_console.Printf(
      "^c%u^a directories, total ^c%s^a bytes.^7\n",
      m_treeEntryList.size(),
      FormatHelper::FormatDecimal(m_totalSize).c_str());
}

unsigned long long TreeViewer::RecursiveGetTreeEntries(const std::string& path, unsigned int depth)
{
   size_t pos = path.find_last_of('\\');

   std::string filespec = path.substr(0, pos) + "\\*.*";

   unsigned long long size = 0;

   _finddata_t findData = { 0 };
   intptr_t handle = _findfirst(filespec.c_str(), &findData);

   if (handle != -1)
   {
      do
      {
         if ((findData.attrib & _A_SUBDIR) != 0)
         {
            // skip names starting with '.'
            if (findData.name[0] == '.')
               continue;

            TreeEntry entry;
            entry.m_name = findData.name;
            entry.m_level = depth;

            std::string subPath = path.substr(0, pos);
            subPath += '\\';
            subPath += findData.name;
            subPath += '\\';

            m_treeEntryList.push_back(entry);

            TreeEntry& insertedEntry = m_treeEntryList.back();

            insertedEntry.m_size = RecursiveGetTreeEntries(subPath, depth + 1);
            size += insertedEntry.m_size;
         }
         else
         {
            size += findData.size;
         }

      } while (_findnext(handle, &findData) == 0);

      _findclose(handle);
   }

   return size;
}

bool TreeViewer::FindNextLevel(unsigned int levelToFind, size_t fromIndex)
{
   auto iter = m_treeEntryList.begin() + fromIndex;

   bool end = false;
   do
   {
      iter++;
      if (iter == m_treeEntryList.end())
         break;

      const TreeEntry& entry = *iter;

      if (entry.m_level == levelToFind)
         return true;

      end = iter == m_treeEntryList.end() ||
         entry.m_level < levelToFind;

   } while (!end);

   return false;
}
