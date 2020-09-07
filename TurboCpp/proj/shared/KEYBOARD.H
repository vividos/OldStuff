// keyboard.h - keyboard access
// (c) 1999 Michael Fink
// https://github.com/vividos/OldStuff/

#ifndef __keyboard_h_
#define __keyboard_h_

#include "keycode.h"

// access to various keyboard states
class keyboard
{
public:
  static bool getbreak(); // kontrolle Åber "CTRL-Break"
  static void setbreak(bool state);
  static bool set_typem(byte delay, byte speed); // setzt Typematic Rate
  static keycode wait_key();
  static bool key_pressed();
  static bool is_special_key(specialKeys sk);
  static keycode upcase(keycode kc);
};

#endif
