//
// HotDir - Colored directory tool
// Copyright (C) 1999-2018 Michael Fink
//
/// \file ConfigFile.hpp Configuration file
//
#pragma once

#include <string>
#include <map>

/// \brief Configuration file
/// \details Stores mapping from color to file extension and can be used to
/// colorize directory file output.
class ConfigFile
{
public:
   /// ctor
   ConfigFile();

   /// Loads configuration from given filename
   void Load(const std::string& filename);

   /// maps file extension to a color to use in file output
   unsigned int GetColorByExtension(const std::string& extensionWithoutDot) const;

private:
   /// parses single line of configuration file
   void ParseLine(std::string line);

   /// parses a line that contains a list of extensions
   void ParseExtensions(unsigned int colorIndex, std::string extensions);

private:
   /// mapping from file extension in lowercase to color number
   std::map<std::string, unsigned int> m_colorByExtension;
};
