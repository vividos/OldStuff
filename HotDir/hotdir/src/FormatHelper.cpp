//
// HotDir - Colored directory tool
// Copyright (C) 1999-2018 Michael Fink
//
/// \file FormatHelper.cpp Text formatting helper
//
#include "pch.h"
#include "FormatHelper.hpp"
#include <io.h>

std::string FormatHelper::FormatDecimal(unsigned long long value)
{
   char buffer[_MAX_U64TOSTR_BASE10_COUNT];
   _ui64toa_s(value, buffer, sizeof(buffer), 10);

   std::string text = buffer;

   for (int pos = static_cast<int>(text.size()) - 3; pos > 0; pos -= 3)
   {
      text.insert(static_cast<size_t>(pos), 1, '.');
   }

   return text;
}

const char* FormatHelper::FormatFileAttributes(unsigned int fileAttributes)
{
   static char help[7];
   strcpy_s(help, sizeof(help), "----- ");

   if ((fileAttributes & _A_SUBDIR) == _A_SUBDIR)
      help[0] = 'd';
   if ((fileAttributes & _A_RDONLY) == _A_RDONLY)
      help[1] = 'r';
   if ((fileAttributes & _A_HIDDEN) == _A_HIDDEN)
      help[2] = 'h';
   if ((fileAttributes & _A_SYSTEM) == _A_SYSTEM)
      help[3] = 's';
   if ((fileAttributes & _A_ARCH) == _A_ARCH)
      help[4] = 'a';

   return help;
}
