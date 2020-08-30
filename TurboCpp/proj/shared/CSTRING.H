// cstring.h - Wrapper-Klasse fuer Strings
// (c) 1998,2020 Michael Fink
// https://github.com/vividos/OldStuff/
// "cstring.h" stellt die Klasse "string" zur Manipulation von Strings bereit.

#ifndef __cstring_h_
#define __cstring_h_

#include "defs.h"

word stringlength(const char* c);
int stringcompare(const char* a, const char* b, word max_length = 0);
char upcase(char c);
char lowcase(char c);

class string
{
private:
  char* sptr; // der String selbst
  word size;  // maximale Laenge
public:
  string(word s);
  string(const char* str);
  string(const char* str, word s);
  string(const string& str);
  ~string();

  // Infos ueber den String
  word length() const { return stringlength(sptr); }; // liefert Laenge des Strings
  word firstpos(char c, bool backward = false) const; // liefert erste Position von 'c'
  word countchar(char c) const; // zaehlt die Anzahl von 'c' im String

   // Umwandlungen zu anderen 'char*'-Typen
  char* getstr(word pos = 0) { return sptr + pos; }; // liefert 'char*' zurueck
  const char* getstr(word pos = 0) const { return sptr + pos; }
  operator const char* () const { return sptr; }; // cast nach 'const char*'
  operator char* () { return sptr; }; // cast nach 'char*'

   // Zuweisungen
  void put(char c, word where) { if (where < size) sptr[where] = c; };
  char get(word where) const { return (where < size) ? sptr[where] : 0; };
  void assign(const char* str) { strcopy(str, 0); }; // weist 'str' zu
  void operator =  (const char* str) { assign(str); }; // Operator = fuer 'char*'
  void operator =  (const string& str) { assign(str.getstr()); }; // fuer 'string&'
  void add(const char* str) { strcopy(str, length()); }; // fgt 'str' hinten an
  void operator += (const char* str) { add(str); }; // addiert einen 'char*'
  void operator += (const string& str) { add(str.getstr()); }; // einen 'string&'
  void operator += (char c) { // oder einen einzelnen 'char'
    word l = length();
    if (l < size)
      put(c, l++), put(0, l);
  };
  void del(word wo, word wieviele = 0)
  {
    if (wieviele == 0) put(0, wo);
    else strcopy(sptr + wo + wieviele, wo);
  };

  // formats text using format string and variable arguments, resizing
  // the string buffer when necessary
  void format(const char* fmt, ...);

  // parses long integer from string
  long get_long() const;

  // Vergleiche; 'int'-Versionen:  0=gleich  -1=sptr<str  1=sptr>str
  int compare(const string& str, int len = 0) const { return compare(str.sptr, len); };
  int compare(const char* str, int len = 0) const { return stringcompare(sptr, str, len); };
  bool operator== (const char* str) { return (bool)(compare(str) == 0); };
  bool operator== (const string& str) { return (bool)(compare(str.getstr()) == 0); };

  // Konvertierung in lower-/uppercase-Strings
  void uppercase();
  void lowercase();

private:
  void strcopy(const char* from, int nach = 0);
};

#endif
