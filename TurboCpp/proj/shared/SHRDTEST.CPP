// shared project unit tests
// (c) 2020 Michael Fink
// https://github.com/vividos/OldStuff/
// Unit tests for shared project files

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

#define STRINGIFY2(x) #x
#define STRINGIFY(x) STRINGIFY2(x)

#define Assert(condition, text) \
  if (!(condition)) \
  { \
    printf("%s(%i): Assert! Error in condition: %s\nMessage: %s", \
      __FILE__, __LINE__, \
      STRINGIFY(condition), text); \
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

#include "cstring.h"

// this tests adding to a string with a fixed buffer size; since
// the buffer is not automatically resized, only a part of the
// added text appears in the string.
void TestStringAdd()
{
  string n("test1", 7);
  n.add("test2");
  Assert(n == "test1t", "string must be added partially");
}

void TestStringFormat()
{
  string s(32); // note that initial buffer is large enough
  s.format("%i + %3.1f = %s", 42, 64.f, "106");
  Assert(s == "42 + 64.0 = 106", "formatted text must match");
}

#include "app.h"
#include "keyboard.h"

class testapp: public app
{
  int count;
public:
  virtual void init()
  {
    count = 0;
  }
  virtual void show_screen();
  virtual void handle_key(keycode kc)
  {
    if (kc == kbF1 &&
        keyboard::is_special_key(skAlt))
      count++;
  }
};

void testapp::show_screen()
{
  point min = {20, 5}, max = {40, 55};
  textwindow* wnd = sc.openwindow(min, max);
  wnd->setattr(white, blue);
  wnd->clear();

  wnd->gotoxy(2, 2);
  wnd->outstr("Hello World!");
  wnd->gotoxy(4, 6);

  string text(40);
  text.format("Count is: %i", count);
  wnd->outstr(text);

  wnd->setattr(white, blue, true);
  wnd->gotoxy(6, 10);
  wnd->outstr("Increase count by pressing Alt+F1!");
}

void TestApp()
{
  testapp app;
  app.run();
}

void main()
{
  clrscr();
  TestLinearMapping();
  TestStringAdd();
  TestStringFormat();
  TestApp();

  printf("All tests OK!");
  getch();
};
