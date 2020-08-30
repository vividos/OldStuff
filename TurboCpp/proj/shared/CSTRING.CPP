// cstring.cpp - Wrapper-Klasse fuer Strings
//(c) 1998,2020 Michael Fink

#include "cstring.h"
#include <mem.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

// gibt Groá- oder Kleinbuchstaben zurck
char upcase(char a)
{
  return (a >= 'a' && a <= 'z') ? a & 0xdf : a;
};

char lowcase(char a) {
  return (a >= 'A' && a <= 'Z') ? a | 0x20 : a;
};

word stringlength(const char* c)
{
  register word ret = 0;
  while (*(c++))
    ret++;
  return ret;
};

int stringcompare(const char* a, const char* b, word max_length)
{
  int diff, count = 0, incr = max_length ? 1 : (max_length = 0xffff, 0);
  do
  {
    if ((diff = *a - *b) != 0) break; // break wenn unterschiedlich
    if (*a == 0 || *b == 0) break;    // break wenn ende
    a++; b++; count += incr;
  } while (count < max_length);
  return diff; // differenz der beiden letzten Zeichen zurckgeben
};


string::string(word s)
{
  sptr = new char[size = s];
  sptr[0] = 0;
};

string::string(const char* str)
{
  sptr = new char[size = stringlength(str) + 1];
  assign(str);
};

string::string(const char* str, word s)
{
  sptr = new char[size = s];
  assign(str);
};

string::string(const string& str)
{
  sptr = new char[size = (word)str.size];
  assign(str.getstr());
};

string::~string()
{
  delete sptr;
  sptr = NULL;
  size = 0;
};

word string::firstpos(char c, bool backward) const
{
  register int i;
  if (!backward)
  {
    i = 0;
    while (get(i) != c && get(i) != 0)i++;
  }
  else
  {
    i = length();
    while (get(i) != c && i != 0)i--;
  };
  return i;
};

word string::countchar(char c) const
{
  word co, i;
  for (co = 0, i = 0; get(i) != 0; i++)
    if (get(i) == c) co++;
  return co;
};

void string::format(const char* fmt, ...)
{
  va_list args;

  va_start(args, fmt);
  int length = vsprintf(NULL, fmt, args);
  va_end(args);

  if (length + 1 > size)
  {
    delete sptr;
    sptr = new char[size = (word)(length + 1)];
  }

  va_start(args, fmt);
  vsprintf(sptr, fmt, args);
  va_end(args);
}

long string::get_long() const
{
  return strtol(getstr(), NULL, 10);
}

void string::strcopy(const char* from, int nach)
{
  word len = stringlength(from);
  if (nach + len >= size) { len = size - nach - 1; sptr[size - 1] = 0; };
  if (len != 0)
    ::memcpy((byte*)sptr + nach, (byte*)from, len);
  sptr[len + nach] = 0;
};

void string::uppercase()
{
  for (register int i = 0; sptr[i] != 0; i++)
    sptr[i] = ::upcase(sptr[i]);
};

void string::lowercase() {
  for (register int i = 0; sptr[i] != 0; i++)
    sptr[i] = ::lowcase(sptr[i]);
};

void string::fill(char ch, int length)
{
  if (length + 1 >= size)
    length = size - 1;
  memset(sptr, ch, length);
}
