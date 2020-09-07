// defs.h - definitions and typedefs
// (c) 1998-1999 Michael Fink
// https://github.com/vividos/OldStuff/
// herein are some popular typedefs to make life a bit easier. there is also
// a (recommended) base class 'object' for all derived classes

#ifndef __defs_h_
#define __defs_h_

// basic typedefs
typedef unsigned char byte;   // 8 bit ohne Vorzeichen
typedef short unsigned word;  // 16 bit ohne Vorz.
typedef unsigned long ulong;  // 32 bit ohne Vorz.
typedef unsigned long dword;  // see ulong

#if !defined(__BORLANDC__) && !defined (_MSC_VER)
  typedef unsigned char bool; // bool for Turbo C++ < v4.x
  #define false 0
  #define true 1
#endif

#ifndef NULL
  #define NULL 0L
#endif

// determines if given value is in a range, including the range borders
#define in_range(value, minvalue, maxvalue) \
  (bool)((value) >= (minvalue) && (value) <= (maxvalue))

// creates a far pointer from given segment and offset values
#define farpointer(seg, ofs) \
  (void far*)(((ulong)(seg) << 16) | (ofs))

// definition of the class object
class object
{
public:
  object() {}; // empty constructor
  virtual ~object() {}; // dtor
  virtual const char* name() { return "object"; }; // name of the object
};

#endif
