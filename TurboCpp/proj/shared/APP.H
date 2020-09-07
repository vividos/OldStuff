// app.h - application base class
// (c) 2020 Michael Fink
// https://github.com/vividos/OldStuff/
// This file contains reconstructed classes from lost source files that
// couldn't be recovered.

#ifndef __app_h_
#define __app_h_

#include "screen.h"
#include "keycode.h"
#include "cstring.h"

// application base class
class app
{
protected:
  // set to true when app should exit
  bool ende;

  // current screen object
  screen sc;

public:
  // ctor
  app();
  // dtor
  virtual ~app() {}

  // wird vor der hauptschleife aufgerufen
  virtual void init();

  // malt den bildschirm neu
  virtual void show_screen();

  // wird aufgerufen, wenn keine taste gedrÅckt wird
  virtual void idle();

  // behandelt tastendrÅcke
  virtual void handle_key(keycode kc);

  // checkt optionen
  virtual void arg_option(const string& opt);

  // parses command line arguments and runs the application
  void run(int argc, char** argv);

  // runs application
  void run();
};

#endif
