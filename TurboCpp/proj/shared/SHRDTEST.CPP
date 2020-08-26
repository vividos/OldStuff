// shared project unit tests
// (c) 2020 Michael Fink
// https://github.com/vividos/OldStuff/
// Unit tests for shared project files

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

// d'oh! this doesn't work in Turbo C++...
#define STRINGIFY(x) #x

#define Assert(condition, text) \
  if (!(condition)) \
  { \
    printf("Assert! Error in condition: %s\n%s", \
      STRINGIFY(x), text); \
      getch(); \
      exit(-1); \
  }

#include "defs.h"

#include "linmap.h"

// tests linear_mapping class
void TestLinearMapping()
{
  linear_mapping m1, m2(0.f, -1.f, 100L, 0L);

  Assert(m1.float2long(0.0) == 0L, "mapped long value must be 0");
  Assert(m1.float2long(0.5) == 50L, "mapped long value must be 50");
  Assert(m1.float2long(1.0) == 100L, "mapped long value must be 100");
  Assert(m1.float2long(42.0) == 4200L, "mapped long value must be 4200");

  Assert(m1.long2float(0L) == 0.f, "mapped float value must be 0.f");
  Assert(m1.long2float(42L) == 0.42f, "mapped float value must be 0.42f");

  Assert(m2.float2long(0.0) == 100L, "mapped long value must be 100");
  Assert(m2.float2long(-0.5) == 50L, "mapped long value must be 50");
  Assert(m2.float2long(1.0) == 200L, "mapped long value must be 200");
}

void main()
{
  clrscr();
  TestLinearMapping();

  printf("All tests OK!");
  getch();
};
