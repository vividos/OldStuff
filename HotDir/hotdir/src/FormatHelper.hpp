//
// HotDir - Colored directory tool
// Copyright (C) 1999-2018 Michael Fink
//
/// \file FormatHelper.hpp Text formatting helper
//
#pragma once

#include <string>

/// \brief Helper methods for formatting text
class FormatHelper
{
public:
   /// Formats integer value with decimal dots
   static std::string FormatDecimal(unsigned long long value);

   /// Formats file attributes as text
   static const char* FormatFileAttributes(unsigned int fileAttributes);

private:
   /// ctor
   FormatHelper() = delete;
};
