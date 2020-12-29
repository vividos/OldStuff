// app.cpp - application base class
// (c) 2020 Michael Fink
// https://github.com/vividos/OldStuff/
// This file contains reconstructed classes from lost source files that
// couldn't be recovered.

#include "app.h"
#include "keyboard.h"

app::app()
{
  ende = false;
}

void app::init()
{
  // nothing to do
}

void app::show_screen()
{
  // nothing to do
}

void app::idle()
{
  // nothing to do
}

void app::handle_key(keycode kc)
{
  if (kc == kbQ && keyboard::is_special_key(skAlt))
    ende = true;
}

#pragma argsused
void app::arg_option(const string& opt)
{
  // nothing to do
}

void app::run(int argc, char** argv)
{
  // start from second argument; first one is the executable
  for (int arg = 1; arg < argc; arg++)
  {
    string s(argv[arg]);
    arg_option(s);
  }

  run();
}

void app::run()
{
  init();
  while (!ende)
  {
    while (!ende && !keyboard::key_pressed())
      idle();

    handle_key(keyboard::wait_key());
    show_screen();
  }
}
