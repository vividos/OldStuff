// cdplayer.cpp - einfache CD-Player-Klasse
// (c) 1998 Michael Fink
// https://github.com/vividos/OldStuff/
// Beschreibung siehe "cdplayer.h"

#include "cdplayer.h"
#include "cdtime.h"

int cdplayer::get_first_track() const
{
  disk_info di;
  get_disk_info(di);
  return di.first;
}

int cdplayer::get_current_track() const
{
  audio_info ai;
  return get_audio_info(ai) ? ai.track : -1;
}

int cdplayer::get_last_track() const
{
  disk_info di;
  get_disk_info(di);
  return di.last;
}

void cdplayer::play_track(int from, int to)
{
  track_info ti;
  get_track_info(from, ti);
  if (to != -1)
  {
    track_info ti2;
    get_track_info(to, ti2);
    ti.end = ti2.end;
  };

  cd_stop_audio();
  cd_play_audio(ti.start, ti.end);
}

void cdplayer::pause_play()
{
  if (is_paused())
    cd_resume_audio();
  else
    cd_stop_audio();
}

void cdplayer::stop_play()
{
  cd_stop_audio();
  cd_stop_audio();
}
