// mscdex.cpp - MSCDEX-Interface
// https://github.com/vividos/OldStuff/
// (c) 1998 Michael Fink
// Beschreibung siehe "mscdex.h"
// MSCDEX details:
// https://gist.github.com/abrasive/7a615e6dde0c1da962f9930cc63ee43d

#include "mscdex.h"
#include <string.h>
#include "cdtime.h"

#pragma option -a-
#pragma option -b-

struct ioctl_io
{
  byte ufktnr;
  union
  {
    // 05 Read Drive Bytes
    struct
    {
      bool drive_open : 1;
      bool drive_unlocked : 1;
      bool raw_and_cooked : 1;
      bool cdrom_can_read_write : 1;
      bool can_audio_too : 1;
      bool can_iso_interleave : 1;
      bool dummy1 : 1;
      bool can_prefetch_sectors : 1;
      bool can_adjust_audio_channels : 1;
      bool can_hsg_redbook : 1;
      bool dummy2 : 1;
      bool cdrom_empty : 1;
      bool can_rw_subchannels : 1;
      byte dummy3 : 3;
      word dummy4 : 16;
    } drivestat;

    // 08 Return Volume Size
    hsg vol_size;

    // 09 Media Changed
    byte changed;

    // 10 Audio Disk Info
    disk_info discinfo;

    // 11 Audio Track Info
    struct
    {
      byte nr;
      redbook start;
      byte stat;
    } track;

    // 14 UPC Code
    struct
    {
      // CONTROL and ADR byte
      byte control_adr;
      // UPC code
      byte code[7];
      byte null, reserved;
    } upc;

    // 15 Audio Status Info
    struct
    {
      // audio status; bit 0: pause; bit 1-15: reserved
      word pause;
      // start and end of next play or resume command
      //redbook start_resume, end_resume;
      hsg start_resume, end_resume;
    } play_info;

    // 12 Audio Q-Channel Info
    struct
    {
      // CONTROL and ADR byte
      byte control_adr;
      byte track, index;
      cd_time tracktime;
      byte zero;
      cd_time disctime;
    } audio_info;

    // in: 4 Audio Channel Info / out: 3 Audio Channel Control
    byte audio_vol[8];
  } par;
} ioctl_io_t;

struct mscdex_query
{
  byte length, command, subunit;
  word statusword;// +03
  byte dummy1[8]; // +05
  byte is_redbook; // +0d // redb=1
  union // +0e
  {
    struct ioctl_io far* ioctl_io_ptr;
    struct
    {
      redbook first, length;
    } play_audio;
  };
} mscdex_query_t;

#pragma option -a
#pragma option -b


mscdex::mscdex()
{
  if (!is_mscdex_installed())
    return;

  mscdex_version = get_mscdex_version();

  for (byte dr = 0; dr < 26; dr++)
    cdrom_drives[dr] = is_cdrom_drive(dr + 'A');

  current_cdrom = first_cdrom_drive() + 'A';
  cd_error = no_error;

  mscdex_anmelden();

  valid_disk_info = false;
}

mscdex::~mscdex()
{
  mscdex_abmelden();
}

void mscdex::get_info(string& s)
{
  if (!is_mscdex_installed())
    s = "no MSCDEX installed";
  else
    s.format("MSCDEX v%i.%i",
      mscdex_version >> 8,
      mscdex_version & 0xff);
}

cd_state mscdex::get_cd_state()
{
  if (!is_mscdex_installed())
    return no_mscdex;

  if (is_tray_open())
    return tray_open;

  if (is_paused())
    return paused;

  mscdex_query mq = { 6, 0, 0xd, 0 }; // 13 DEVICE OPEN
  send_query(&mq);

  return (mq.statusword & 0x200) == 0 ? stopped : playing;
}

bool mscdex::get_disk_info(disk_info& di)
{
  if (valid_disk_info)
  {
    di = cached_disk_info;
    return true;
  }

  ioctl_io io = { 10 }; // Audio Disk Info
  bool ret = ioctl_input(&io);

  if (ret)
  {
    di.first = io.par.discinfo.first;
    di.last = io.par.discinfo.last;
    di.leadout = io.par.discinfo.leadout;

    valid_disk_info = true;
    cached_disk_info = di;
  }

  return ret;
}

bool mscdex::get_track_info(byte track, track_info& ti)
{
  disk_info di;
  get_disk_info(di);
  if (track > di.last)
    return false;

  ioctl_io io = { 11 }; // Audio Track Info
  io.par.track.nr = track;
  if (!ioctl_input(&io))
    return false;

  ti.start = io.par.track.start;
  ti.data = (bool)((io.par.track.stat & 0x40) != 0);
  ti.copy = (bool)((io.par.track.stat & 0x20) != 0);

  if (track == di.last)
    ti.end = di.leadout - 1;
  else
  {
    ioctl_io io2 = { 11 }; // Audio Track Info
    io2.par.track.nr = track + 1; // next track

    if (!ioctl_input(&io2))
      return false;
    ti.end = io2.par.track.start;
  }

  return true;
}

bool mscdex::get_audio_info(audio_info& ai)
{
  ioctl_io io = { 12 }; // Audio Q-Channel Info
  bool ret = ioctl_input(&io);

  ai.track = io.par.audio_info.track + 1; // no BCD
  ai.tracktime = io.par.audio_info.tracktime;
  ai.disktime = io.par.audio_info.disctime;

  return ret;
}

bool mscdex::media_changed()
{
  ioctl_io io = { 9 }; // Media Changed
  bool ret = ioctl_input(&io);

  bool media_changed =
    ret ? (io.par.changed == 1 ? false : true) : false;

  if (media_changed)
    valid_disk_info = false;

  return media_changed;
}

bool mscdex::scan_upc(byte* upc)
{
  ioctl_io io = { 14 }; // UPC Code
  ioctl_input(&io);

  memcpy((byte*)upc, (byte*)io.par.upc.code, 7);
  return (bool)(io.par.upc.control_adr != 0);
}

redbook mscdex::get_cd_length()
{
  disk_info di;
  bool ret = get_disk_info(di);

  return ret ? di.leadout : 0;
}

bool mscdex::get_volume(byte* volume, byte* inputs)
{
  ioctl_io io = { 4 }; // Audio Channel Info
  bool ret = ioctl_input(&io);

  for (int i = 0; i < 4; i++)
  {
    volume[i] = io.par.audio_vol[(i << 1) + 1];
    inputs[i] = io.par.audio_vol[i << 1];
  }

  return ret;
}

bool mscdex::cd_play_audio(redbook start, redbook end)
{
  mscdex_query mq = { 22, 0, 0x84, 0 }; // 132 PLAY AUDIO
  mq.is_redbook = true;
  mq.play_audio.first = start;
  mq.play_audio.length = end - start;

  return send_query(&mq);
}

bool mscdex::cd_stop_audio()
{
  mscdex_query mq = { 6, 0, 0x85, 0 }; // 133 STOP AUDIO
  return send_query(&mq);
}

bool mscdex::cd_resume_audio()
{
  mscdex_query mq = { 6, 0, 0x88, 0 }; // 136 RESUME AUDIO
  return send_query(&mq);
}

bool mscdex::set_volume(const byte* volume, const byte* inputs)
{
  ioctl_io io = { 3 }; // Audio Channel Control
  for (int i = 0; i < 4; i++)
  {
    io.par.audio_vol[(i << 1) + 1] = volume[i];
    io.par.audio_vol[i << 1] = inputs[i];
  }

  return ioctl_output(&io);
}

bool mscdex::open_or_close_tray()
{
  valid_disk_info = false;

  cd_stop_audio();
  cd_stop_audio();

  ioctl_io io = { 0 }; // Eject Disk
  if (is_tray_open())
    io.ufktnr = 5; // Close Tray

  return ioctl_output(&io);
}

bool mscdex::is_mscdex_installed()
{
  asm {
    mov ax, 1500h // Get Number of CD-ROM Drive Letters
    xor bx, bx
    int 2fh
  };

  return _BX != 0;
}

word mscdex::get_mscdex_version()
{
  asm {
    mov ax, 150ch // MSCDEX Version
    xor bx, bx
    int 2fh
  };

  word ver = _BX;
  return ver == 0 ? 0x01ff : ver;
}

word mscdex::how_many_cdrom_drives()
{
  asm {
    mov ax, 1500h // Get Number of CD-ROM Drive Letters
    int 2fh
  };

  return _BX;
}

char mscdex::first_cdrom_drive()
{
  asm {
    mov ax, 1500h // Get Number of CD-ROM Drive Letters
    int 2fh
  };

  return _CL;
}

bool mscdex::is_cdrom_drive(char drive)
{
  asm {
    mov ax, 150bh // CD-ROM Drive Check
    mov cl, drive
    xor ch, ch
    dec cx
    and cx, 01fh
    int 2fh
  };

  return (bool)(_BX == 0xadad && _AX != 0);
}

bool mscdex::send_query(mscdex_query far* q)
{
  if (mscdex_version < 0x210)
    return false;

  _CX = current_cdrom - 'A';
  asm {
    mov ax, 1510h // Send Device Driver Request
    les bx, q
    int 2fh
  };

  if (((word)q->statusword) < 0x8000)
    cd_error = no_error;
  else
    cd_error = (cderrors)(q->statusword & 0xff);

  return (bool)(cd_error == no_error);
}

bool mscdex::mscdex_anmelden()
{
  mscdex_query mq = { 6, 0, 0xd, 0 }; // 13 DEVICE OPEN
  return send_query(&mq);
}

bool mscdex::mscdex_abmelden()
{
  mscdex_query mq = { 6, 0, 0xe, 0 }; // 14 DEVICE CLOSE
  return send_query(&mq);
}

bool mscdex::ioctl_input(ioctl_io* io)
{
  mscdex_query mq = { 18, 0, 0x3, 0 }; // 3 IOCTL INPUT
  mq.ioctl_io_ptr = io;
  return send_query(&mq);
}

bool mscdex::ioctl_output(ioctl_io* io)
{
  mscdex_query mq = { 18, 0, 0xc, 0 }; // 12 IOCTL OUTPUT
  mq.ioctl_io_ptr = io;
  return send_query(&mq);
}

bool mscdex::is_tray_open()
{
  ioctl_io io = { 6 }; // Device Status
  ioctl_input(&io);

  return io.par.drivestat.drive_open;
}

bool mscdex::is_cd_inside()
{
  ioctl_io io = { 6 }; // Device Status
  ioctl_input(&io);

  return !io.par.drivestat.cdrom_empty;
}

bool mscdex::is_paused()
{
  ioctl_io io = { 15 }; // Audio Status Info
  ioctl_input(&io);

  return (bool)(io.par.play_info.pause == 1);
}

bool mscdex::skip_secs(int seconds)
{
  if (!is_paused())
    cd_stop_audio();

  cd_time_convert cc;

  ioctl_io io = { 15 }; // Audio Status Info
  ioctl_input(&io);

  cc.in_redbook(io.par.play_info.start_resume);
  hsg start_hsg = cc.out_hsg();

  cc.in_redbook(io.par.play_info.end_resume);
  hsg ende_hsg = cc.out_hsg();

  if (((long)start_hsg + seconds * 75) > 0)
    start_hsg += seconds * 75;
  else
    start_hsg = 0;

  if (start_hsg > ende_hsg)
    start_hsg = ende_hsg - 1;

  redbook new_start;
  cc.in_hsg(start_hsg);
  new_start = cc.out_redbook();

  return cd_play_audio(new_start, io.par.play_info.end_resume);
}
