//
// HotDir - Colored directory tool
// Copyright (C) 1999-2018 Michael Fink
//
/// \file ConfigFile.cpp Configuration file
//
#include "pch.h"
#include "Configfile.hpp"
#include <fstream>
#include <array>

const unsigned int DefaultColor = 7; ///< white

/// all color names that can be used in the config file
std::array<const char*, 16> ColorNames =
{
   "black", "blue", "green", "cyan", "red", "magenta",
   "brown", "lightgray", "darkgray", "lightblue", "lightgreen", "lightcyan",
   "lightred", "lightmagenta", "yellow", "white"
};

ConfigFile::ConfigFile()
{
}

void ConfigFile::Load(const std::string& filename)
{
   std::ifstream configFile(filename);

   if (!configFile.is_open())
      return;

   std::string line;
   while (!configFile.eof())
   {
      std::getline(configFile, line);

      ParseLine(line);
   }

   configFile.close();
}

unsigned int ConfigFile::GetColorByExtension(const std::string& extensionWithoutDot) const
{
   auto iter = m_colorByExtension.find(extensionWithoutDot);

   return iter != m_colorByExtension.end() ? iter->second : DefaultColor;
}

void ConfigFile::ParseLine(std::string line)
{
   // remove comments
   size_t pos = line.find_first_of(";#");
   if (pos != std::string::npos)
      line = line.substr(0, pos);

   // remove whitespace at start
   while (line.size() > 0 &&
      isspace(line[0]))
   {
      line.erase(0, 1);
   }

   pos = line.find('=');
   if (pos == std::string::npos)
      return;

   std::string color = line.substr(0, pos);
   std::string extensions = line.substr(pos + 1);

   if (extensions.empty())
      return;

   auto colorPos = std::find(ColorNames.begin(), ColorNames.end(), color);
   if (colorPos == ColorNames.end())
      return; // invalid color name

   unsigned int colorIndex = static_cast<unsigned int>(std::distance(ColorNames.begin(), colorPos));

   ParseExtensions(colorIndex, extensions);
}

void ConfigFile::ParseExtensions(unsigned int colorIndex, std::string extensions)
{
   size_t pos = extensions.find_first_of(',');
   while (pos != std::string::npos)
   {
      std::string extension = extensions.substr(0, pos);

      std::transform(extension.begin(), extension.end(), extension.begin(),
         [](char ch) -> char { return static_cast<char>(tolower(ch)); });

      m_colorByExtension.insert(std::make_pair(extension, colorIndex));

      extensions.erase(0, pos + 1);
      pos = extensions.find_first_of(',');
   }
}
