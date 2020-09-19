// cdp.cpp - cd-player v1.4
// https://github.com/vividos/OldStuff/
// (c) 1998, 2020 Michael Fink

#include "cdplayer.h"
#include "cdtime.h"
#include "app.h"
#include "window.h"
#include "keyboard.h"
#include <assert.h>
#include <dos.h> // for delay()

// CD player application
class cdp_app : public app
{
  // cd player instance
  cdplayer* player;

  // indicates if window is drawn for the first time
  bool show_first_time;

  // text window
  textwindow* wi;

  // indicates if a new track is now playing
  bool newtrack;

  // UPC code for current disc
  string upc;

  // indicates if UPC code is read at start
  bool upc_at_start;

  // current CD state
  cd_state state;

  // indicates if track and disc times should be negated
  bool track_neg, disc_neg;

  // volume levels and input channels
  byte volume[4], inputs[4];

  // current volume/inputs cursor position
  byte crsr;

  // indicates if a new CD was inserted
  bool new_cd;

public:
  // ctor
  cdp_app();

  // dtor
  virtual ~cdp_app()
  {
    delete player;
    delete wi;
  }

  virtual void arg_option(const string& opt);
  virtual void init();
  virtual void show_screen();
  virtual void handle_key(keycode kc);
  virtual void idle();

  void cd_info();

private:
  void draw_window();
  void update_window();
  bool scan_upc();
  byte input_new_track();
};

cdp_app::cdp_app()
  :show_first_time(true),
  upc(14)
{
  wi = NULL;
  player = new cdplayer;

  newtrack = false;
  upc_at_start = false;
  track_neg = disc_neg = false;
  crsr = 0;
  new_cd = false;
}

void cdp_app::init()
{
  app::init();

  if (player->get_error() != no_error &&
    player->get_error() != non_writeable)
  {
    sc.outstr("no cdrom-drive found!");
    ende = true;
    return;
  }

  point min = { 23, 3 }, max = { 57, 22 };
  wi = sc.openwindow(min, max);
  wi->setattr(yellow, blue);

  sc.store();
  state = player->get_cd_state();
  scan_upc();
  player->get_volume(volume, inputs);
}

void cdp_app::show_screen()
{
  if (show_first_time)
  {
    show_first_time = false;

    draw_window();
  }

  update_window();
}

void cdp_app::draw_window()
{
  wi->clear();
  wi->gotoxy(1, 1);
  wi->outstr(
    " ÉÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ» "
    "Ú¶ CD-Player v1.4                Ç¿"
    "°º Track: xx/xx   xx:xx:xx [F9]  º°"
    "°º Disc:          xx:xx:xx [F10] º°"
    //"°º Repeat: all__ [F11]           º°"
    "°ÇÄ keys: ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¶°"
    "°º  +/-   change track           º°"
    "°º  F1    new track              º°"
    "°º  F2    pause playing          º°"
    "°º  F3    stop                   º°"
    "°º  F4    open/close tray        º°"
    "°º  F5/F8 skip 5 sec. back/forw. º°"
    "°º  F6/F7 skip 15 sec.           º°"
    "°ÌÄ out ÄÄÄÄ volume ÄÄ input ÄÄÄÄ¹°"
    "°º  left  [<] xxx [>]   x [1..4] º°"
    "°º right      xxx       x        º°"
    "°º quad1      xxx       x        º°"
    "°º quad2      xxx       x        º°"
    "°ÇÄ Status: ÄÄÄÄÄÂÄ UPC ÄÄÄÄÄÄÄÄÄ¶°"
    "À¶               ³ xxxxxxxxxxxxx ÇÙ"
    " ÈÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÏÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ¼"
  );
  sc.setcursortype(nocursor);
}

void cdp_app::update_window()
{
  string s(80);

  audio_info ai;
  player->get_audio_info(ai);
  wi->gotoxy(11, 3);

  if (!newtrack)
  {
    if (player->is_tray_open())
      s = "--/--";
    else
      s.format("%02.2i/%02.2i", ai.track, player->get_last_track());
    wi->outstr(s);
  }

  cd_time_convert cc;

  // Track-Time
  track_info tr;
  player->get_track_info(ai.track, tr);

  cc.in_time(ai.tracktime);
  ulong track_frames = tr.end - tr.start;
  byte pos1 = track_frames == 0 ? 0
    : 15 * cc.out_frame() / track_frames;

  if (track_neg)
  {
    cc.in_time(ai.tracktime);
    cc.negate(tr.end);
    ai.tracktime = cc.out_time();
  }

  wi->gotoxy(18, 3);

  cc.in_time(ai.tracktime);
  cc.format(s, track_neg);
  wi->outstr(s);

  // Disc-Time
  player->get_track_info(player->get_last_track(), tr);
  cc.in_redbook(player->get_cd_length());
  ulong len = cc.out_hsg();

  cc.in_time(ai.disktime);
  byte pos2 = len == 0 ? 0 : 15 * cc.out_hsg() / len;

  if (disc_neg)
  {
    cc.in_time(ai.disktime);
    cc.negate(tr.end);
    ai.disktime = cc.out_time();
  }

  wi->gotoxy(18, 4);
  cc.in_time(ai.disktime);
  cc.format(s, disc_neg);
  wi->outstr(s);

  // balken zeichnen
  for (int j = 0; j < 16; j++)
  {
    wi->gotoxy(1, j + 3);  wi->outstr(j == pos1 ? "ş" : "°");
    wi->gotoxy(35, j + 3); wi->outstr(j == pos2 ? "ş" : "°");
  }

  // volume-anzeige
  for (int i = 0; i < 4; i++)
  {
    const char* text;
    switch (i)
    {
    case 0: text = "left"; break;
    case 1: text = "right"; break;
    case 2: text = "quad1"; break;
    case 3: text = "quad2"; break;
    }

    s.format("%5.5s  %3.3s %3.3i %3.3s   %1.1i %6s",
      text,
      crsr == i ? "[<]" : "",
      volume[i],
      crsr == i ? "[>]" : "",
      inputs[i] + 1,
      crsr == i ? "[1..4]" : "");

    wi->gotoxy(4, 14 + i);
    if (crsr == i)
      wi->setattr(blue, green);
    wi->outstr(s);
    if (crsr == i)
      wi->setattr(yellow, blue);
  }

  // status
  wi->gotoxy(4, 19);
  switch (state)
  {
  case empty_drive: wi->outstr("no cdrom found"); break;
  case playing:  wi->outstr("playing ...   "); break;
  case paused:   wi->outstr("pause ...     "); break;
  case stopped:  wi->outstr("stopped.      "); break;
  case tray_open:wi->outstr("tray is open. "); break;
  }

  // upc
  wi->gotoxy(20, 19);
  wi->outstr(upc);
}

void cdp_app::handle_key(keycode kc)
{
  byte track = player->get_current_track();
  switch (kc)
  {
  case kbF1:
    byte tr = input_new_track();
    if (in_range(tr, player->get_first_track(), player->get_last_track()))
      player->play_track(tr);
    break;

  case kbF2:
    player->pause_play();
    break;

  case kbF3:
    player->stop_play();
    if (upc.get(0) == '-')
      scan_upc();
    break;

  case kbF4:
    bool zu = player->is_tray_open();
    player->open_or_close_tray();
    if (zu)
      player->play_track(1);
    break;

  case kbF5:
    player->skip_secs(-15);
    break;

  case kbF6:
    player->skip_secs(-5);
    break;

  case kbF7:
    player->skip_secs(5);
    break;

  case kbF8:
    player->skip_secs(15);
    break;

  case kbF9:
    track_neg = (bool)(!track_neg);
    break;

  case kbF10:
    disc_neg = (bool)(!disc_neg);
    break;

  case 0x7000:
    if (upc.get(0) == '-')
      scan_upc();
    break;

  case kbPlus:
    if (track < player->get_last_track())
      player->play_track(++track);
    break;

  case kbMinus:
    if (track > player->get_first_track())
      player->play_track(--track);
    break;

  case kbDown:
    if (crsr != 3)
      crsr++;
    else crsr = 0;
    break;

  case kbUp:
    if (crsr != 0)
      crsr--;
    else crsr = 3;
    break;

  case kbLeft:
    int dec = keyboard::is_special_key(skShift) ? 1 : 16;
    if (volume[crsr] < dec)
      volume[crsr] = 0;
    else
      volume[crsr] -= dec;
    player->set_volume(volume, inputs);
    break;

  case kbRight:
    int inc = keyboard::is_special_key(skShift) ? 1 : 16;
    if ((int)volume[crsr] + inc > 255)
      volume[crsr] = 255;
    else
      volume[crsr] += inc;
    player->set_volume(volume, inputs);
    break;

  case kb1: inputs[crsr] = 0; player->set_volume(volume, inputs); break;
  case kb2: inputs[crsr] = 1; player->set_volume(volume, inputs); break;
  case kb3: inputs[crsr] = 2; player->set_volume(volume, inputs); break;
  case kb4: inputs[crsr] = 3; player->set_volume(volume, inputs); break;

  case kbEsc:
    ende = true;
    break;
  }
}

void cdp_app::idle()
{
  if (new_cd)
  {
    scan_upc();
    player->play_track(1);
    if (player->get_cd_state() == playing)
      new_cd = false;
  }

  cd_state oldstate = state;
  state = player->get_cd_state();
  if (oldstate == tray_open && state != oldstate)
    new_cd = true;

  show_screen();

  // delay the main loop so that playback doesn't get choppy
  delay(100);
}

bool cdp_app::scan_upc()
{
  byte code[7];
  bool result = player->scan_upc(code);
  if (result)
  {
    for (int i = 0; i < 7; i++)
    {
      upc.put((code[i] >> 4) | 0x30, i * 2);
      upc.put((code[i] & 15) | 0x30, i * 2 + 1);
    }
  }
  else
    upc.fill('-', 13);

  upc.put(0, 13);
  return result;
}

byte cdp_app::input_new_track()
{
  newtrack = true;
  byte track = player->get_current_track();
  wi->gotoxy(11, 3);
  wi->outstr("new: __");
  wi->gotoxy(16, 3);

  byte maxtracks = player->get_last_track();

  string s(2);
  char c;
  byte b = 0;

  do
  {
    do
    {
      show_screen();
    } while (!keyboard::key_pressed());

    s += c = keyboard::wait_key();
    b++;
    if (c == 0xd)
    {
      if (b == 2)
        track = s.get(0) - '0';
      break;
    }

    if (c == 27)
    {
      track = 0;
      break;
    }

    wi->gotoxy(16, 3);
    wi->outstr(s);

    if (b == 1)
    {
      if (maxtracks < 10 || c - '0' > maxtracks / 10)
      {
        track = c - '0';
        break;
      }
    }

    if (b == 2)
    {
      track = s.get_long();
      break;
    }
  } while (true);

  newtrack = false;
  wi->gotoxy(11, 3);
  wi->outstr("       ");

  return track;
}

// Kommandozeilen-Optionen
void cdp_app::arg_option(const string& opt)
{
  switch (upcase(opt.get(1)))
  {
  case 'H':
  case '?':
    ende = true;
    sc.outstr(
      "CD-Player v1.4 Help\n"
      "Options:\n"
      "  -info     get info about mscdex and cd\n"
      "  -trackXX  play track XX\n"
      "  -pause    pause cd-playing (and resume)\n"
      "  -stop     stop playing cd\n"
      "  -open     open or close cd-tray\n"
      "  -upc      scan Universal Product Code at start\n"
    );
    break;

  case 'I':
    ende = true;
    cd_info();
    break;

  case 'T':
    ende = true;
    string tracktext(opt.getstr(6));
    player->play_track((int)tracktext.get_long());
    break;

  case 'S':
    ende = true;
    player->stop_play();
    break;

  case 'P':
    ende = true;
    player->pause_play();
    break;

  case 'O':
    ende = true;
    player->open_or_close_tray();
    break;

  case 'U':
    upc_at_start = true;
    break;

  default:
    sc.outstr("Invalid option specified!\n");
    break;
  }
}

void cdp_app::cd_info()
{
  string s("CD-Player v1.4 CD-Info\n", 80);
  sc.outstr(s);

  player->get_info(s);
  sc.outstr(s);
  sc.crlf();
  s.del(0);

  state = player->get_cd_state();

  if (state == empty_drive)
    return;

  if (state != tray_open)
  {
    cd_time_convert cc;
    cc.in_redbook(player->get_cd_length());
    cd_time cdt = cc.out_time();

    string disctext(80);
    disctext.format("disc has %i tracks and is %02.2i:%02.2i:%02.2i long.\n",
      player->get_last_track(), cdt.min, cdt.sec, cdt.frame);
    s.add(disctext);
  }

  switch (state)
  {
  case tray_open:
    s += "tray is open.";
    break;

  case playing:
    audio_info ai;
    player->get_audio_info(ai);
    string playtext(80);
    playtext.format(
      "cd is playing track %02.2i, at %02.2i:%02.2i:%02.2i.",
      ai.track,
      ai.tracktime.min,
      ai.tracktime.sec,
      ai.tracktime.frame);
    s.add(playtext);
    break;

  case paused:
    s += "cd is paused.";
    break;

  case stopped:
    s += "cd is stopped.";
    break;

  default:
    assert(false); // invalid state
    break;
  }

  s += '\n';
  sc.outstr(s);
}

void main(int argc, char** argv)
{
  cdp_app p;
  p.run(argc, argv);
};
