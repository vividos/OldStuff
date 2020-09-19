// cdtime.h - CD time converter
// https://github.com/vividos/OldStuff/
// (c) 1998 Michael Fink
// Converts between different CD time formats
#pragma once

#ifndef __cdtimeconvert_h_
#define __cdtimeconvert_h_

#include "cstring.h"

class cd_time_convert
{
  cd_time time;
public:
  cd_time_convert() {}
  void in_time(cd_time _time) { time = _time; }
  void in_frame(ulong fr)
  {
    time.min = (byte)(fr / (60L * 75L));
    fr -= time.min * 60L * 75L;

    time.sec = (byte)(fr / (1L * 75L));
    fr -= time.sec * 1L * 75L;

    time.frame = (byte)fr;
  };

  void in_redbook(redbook rb)
  {
    time.frame = (byte)(rb >> 0);
    time.sec = (byte)(rb >> 8);
    time.min = (byte)(rb >> 16);
  }

  void in_hsg(hsg h)
  {
    in_frame(h + 150);
  };

  // negates a time value by using given end time as "mirror" value
  void negate(hsg end)
  {
    hsg h = out_hsg();
    h = end - h;
    in_hsg(h);
  }

  void format(string& s, bool negative)
  {
    s.format("%c%02.2i:%02.2i:%02.2i",
      negative ? '-' : ' ',
      time.min,
      time.sec,
      time.frame);
  }

  cd_time out_time() const
  {
    return time;
  }

  ulong out_frame() const
  {
    return (time.min * 60L + time.sec) * 75L + time.frame;
  }

  redbook out_redbook() const
  {
    return ((ulong)time.min << 16) | ((ulong)time.sec << 8) | ((ulong)time.frame);
  }

  hsg out_hsg() const
  {
    return out_frame() - 150L;
  }
};

#endif
