// mscdex.h - MSCDEX-Interface
// https://github.com/vividos/OldStuff/
// (c) 1998 Michael Fink
//
// Beschreibung:
//  Die Klasse 'mscdex' enthÑlt ein komplettes Interface zum CD-ROM-
//  DOS-Treibers MSCDEX, sowie auch Zugriff auf Funktionen des eigentlichen
//  CD-ROM-Treibers.
//
#pragma once

#ifndef __mscdex_h_
#define __mscdex_h_

#include "defs.h"
#include "cstring.h"

 // die einzelnen Fehler, die auftreten kînnen
enum cderrors
{
  non_writeable = 0,
  unknown_drive = 1,
  no_cd_found = 2,
  unknown_command = 3,
  crc_error = 4,
  wrong_param_data_len = 5,
  seek_error = 6,
  wrong_cdrom_format = 7,
  sector_not_found = 8,
  printer_out_of_paper = 9,
  write_error = 10,
  read_error = 11,
  general_error = 12,
  reserved1 = 13,
  no_media_inside = 14,
  media_changed_error = 15,
  no_cdrom_installed = 0xfe,
  no_error = 0xff
};

// die verschiedenen ZustÑnde, die das CD-ROM-Laufwerk annehmen kann
enum cd_state
{
  no_mscdex = 0,
  empty_drive,
  tray_open,
  stopped,
  paused,
  playing
};

#pragma option -a-
#pragma option -b-

// CD-Spielzeiten werden meist im Redbook-Format Åbergeben
// format: 0x00MMSSFF (MM: minutes, SS: seconds, FF: frames)
typedef ulong redbook;

// HSG ist ein etwas anderes Format, CD-Zeiten zu Åbergeben
// HSG = High Sierra (logical block numbers)
typedef ulong hsg;

// es gibt auch die einfachere Schreibweise
struct cd_time { byte min, sec, frame; };

// diese Informationen bekommt man von get_disk_info() zurÅck
struct disk_info
{
  byte first, last; // Track-Nummern, 1-basiert
  redbook leadout;  // Anfang des lead-out-Tracks hinter dem letzten Track
};

// die Track-Informationen von get_track_info()
struct track_info
{
  redbook start, end; // Start und Ende des Tracks
  bool data; // gibt an, ob es ein Daten-Track ist
  bool copy; // wenn true, darf der Track digital kopiert werden
};

// Infos Åber die aktuelle CD-Wiedergabe
struct audio_info
{
  byte track; // 1-basiert
  cd_time tracktime, disktime;
};

#pragma option -a
#pragma option -b

struct mscdex_query;
struct ioctl_io;

// Interface fÅr MSCDEX
class mscdex
{
private:
  cderrors cd_error;   // eventuell auftretende Fehler
  word mscdex_version; // die Versionsnummer: vHI.LO

  bool cdrom_drives[26];// ein Array, welche Laufwerksbuchstaben CD-ROMs sind
  char current_cdrom;   // das aktuell eingestellte Laufwerk

  bool valid_disk_info;
  disk_info cached_disk_info;

public:
  mscdex();
  ~mscdex();

  void get_info(string& s);

  // status
  cderrors get_error() const { return cd_error; }
  cd_state get_cd_state(); // liefert den Zustand des Laufwerks zurÅck
  bool get_disk_info(disk_info& di); // holt die Infos Åber die CD
  bool get_track_info(byte track, track_info& ti); // ... Åber den Track
  bool get_audio_info(audio_info& ti); // ... Åber die Audio-Wiedergabe
  bool media_changed();     // gibt an, ob die CD gewechselt wurde
  bool scan_upc(byte* upc); // liefert den Universal Product Code
  redbook get_cd_length();  // liefert die LÑnge der CD im Redbook-Format
  // holt die LautstÑrken und die EingangskanÑle fÅr die AusgangskanÑle 0-3
  bool get_volume(byte* volume, byte* inputs);

  bool is_tray_open();
  bool is_cd_inside();
  bool is_paused();

  // cd-funktionen
  bool cd_play_audio(redbook start, redbook end); // spielt einen Bereich ab
  bool cd_stop_audio();   // stoppt die Wiedergabe; 2x fÅr Stop
  bool cd_resume_audio(); // setzt die Wiedergabe nach cd_stop_audio() fort

  // einstellungen
  // setzt die LautstÑrken und EingangskanÑle
  bool set_volume(const byte* volume, const byte* inputs);
  bool open_or_close_tray(); // schlie·t oder îffnet das Laufwerk (je nachdem)
  bool skip_secs(int seconds);

private:
  // mscdex-anfragen
  bool is_mscdex_installed();
  word get_mscdex_version();
  word how_many_cdrom_drives();
  char first_cdrom_drive();
  bool is_cdrom_drive(char drive);

  // anfragen Åber send_query
  bool send_query(mscdex_query far* q);
  bool mscdex_anmelden();
  bool mscdex_abmelden();

  // anfragen Åber ioctl
  bool ioctl_input(ioctl_io* io);
  bool ioctl_output(ioctl_io* io);
};

#endif
