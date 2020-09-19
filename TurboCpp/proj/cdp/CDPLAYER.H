// cdplayer.h - einfache CD-Player-Klasse
// (c) 1998 Michael Fink
// https://github.com/vividos/OldStuff/
// Die Klasse 'cdplayer' implementiert einen kleinen CD-Player, basierend
// auf MSCDEX.EXE
#pragma once

#ifndef __cdplayer_h_
#define __cdplayer_h_

#include "mscdex.h"

// high-level CD player methods
class cdplayer : public mscdex
{
public:
  int get_first_track() const;
  int get_current_track() const;
  int get_last_track() const;

  void play_track(int from, int to = -1); // spielt den Track, oder bis zu 'to'
  void pause_play(); // pausiert das Abspielen
  void stop_play(); // stoppt das Abspielen
};

#endif
