// vesamode.cpp - VESA graphics mode
// (c) 1997, 2020 Michael Fink
// https://github.com/vividos/OldStuff/
// See also: https://web.archive.org/web/20081211174813/http://docs.ruudkoot.nl/vbe20.txt
// And: http://www.monstersoft.com/tutorial1/VESA_intro.html

#pragma once

#include "vesamode.h"
#include "cstring.h"
#include <stdio.h>
#include <mem.h>
#include <assert.h>

extern void set_vga_palette(const palette256 palette);

#pragma option -a-
#pragma option -b-

// also known as VbeInfoBlock
struct vesa_vbe_info_block
{
  char signature[4];
  byte sub_version, main_version;
  char far* oem_string;
  ulong capabilities;
  word far* video_mode_list;
  byte reserved2[0xff - 0x12];
}; // 256 bytes

// also known as ModeInfoBlock
struct vesa_mode_info_block
{
  // VBE all versions
  struct
  {
    bool mode_with_mon : 1; // D0
    bool opt_infos : 1;
    bool bios_text : 1;
    bool color_mode : 1; // D3
    bool graph_text : 1; // D4
    bool vga_compat : 1;

    // D6: indicates if windowed (or banked) memory access is supported
    bool vga_window_memory : 1;

    // D7: indicates if linear framebuffer is supported
    bool linear_framebuffer : 1;
    word unused : 8;
  } modeflag;

  // frame flags for window 1 and 2
  struct
  {
    bool avail1 : 1;
    bool read1 : 1;
    bool write1 : 1;
    bool unused1 : 5;
    bool avail2 : 1;
    bool read2 : 1;
    bool write2 : 1;
    bool unused2 : 5;
  } frameflags;

  // window granularity, in kb
  word granularity_kb;

  // window or frame size, in kb
  word framesize_kb;

  // segment addresses for window 1 and 2
  word seg1, seg2;

  // pointer to window function
  void (far* setframe)();

  // bytes per scanline
  word bytes_per_line;

  // VBE 1.2 and above

  // resolution
  word xres, yres;

  // character cell size
  byte xchar, ychar;

  // number of memory planes
  byte num_planes;
  byte bits_per_pixel;
  byte num_banks;

  enum memory_model
  {
    text = 0,
    CGA,
    hercules,
    mode4bit,
    mode8bit,
    mode4bitpacked,
    hiTrueColor
  } model;

  byte bank_size_kb;

  byte num_images;

  byte reserved_vbe12;

  byte color_mask_and_pos[8];

  byte direct_color_mode_info;  

  // VBE 2.0

  // physical address for flat or linear frame buffer
  byte far* flat_frame_buffer;

  ulong offscreen_memory_offset;
  word offscreen_memory_size_kb;
  byte reserved_vbe20[206];
}; // 256 bytes

#pragma option -b
#pragma option -a

bool vesa_mode::get_vbe_info_block(vesa_vbe_info_block far* vib)
{
  asm {
    push di
    mov ax, 0x4f00
    les di, vib
    int 10h
    pop di
  };

  return (bool)(_AL == 0x4f);
}

word vesa_mode::get_current_vesa_mode()
{
  asm {
    mov ax, 0x4f03
    int 10h
  };
  return (word)_BX;
}

bool vesa_mode::get_mode_info_block(vesa_mode_info_block far* mib, word mode)
{
  assert(sizeof(vesa_mode_info_block) == 256);

  // 4f01h: Return VBE mode information
  asm {
    mov ax, 0x4f01
    mov cx, mode
    les di, mib
    int 10h
  };

  return (bool)(_AL == 0x4f);
}

bool vesa_mode::is_avail()
{
  vesa_vbe_info_block vib = { 0 };
  return get_vbe_info_block(&vib);
}

void vesa_mode::show_modes()
{
  vesa_vbe_info_block vib = { 0 };
  bool avail = get_vbe_info_block(&vib);

  if (!avail)
  {
    printf("no vesa-driver found.\n");
    return;
  };

  string s(80);
  s.format(" %.4Fs v%i.%i >%Fs<\n",
    vib.signature, vib.main_version, vib.sub_version, vib.oem_string);
  printf(s);

  printf("  mode gr/t xres  yres bit  BpL pl gran frame framebuffer\n");

  word far* mode_ptr = vib.video_mode_list;

  vesa_mode_info_block mib = { 0 };

  while (*mode_ptr != (word)-1)
  {
    word mode = *mode_ptr;
    if (get_mode_info_block(&mib, mode))
    {
      s.format("%5xh %4s%5i%6i%4i%5i%3i%4ik%5ik",
        mode,
        mib.modeflag.graph_text ? "graf" : "text",
        mib.xres,
        mib.yres,
        mib.bits_per_pixel,
        mib.bytes_per_line,
        mib.num_planes,
        mib.granularity_kb,
        mib.framesize_kb
      );

      printf(s.getstr());

      if (mib.modeflag.linear_framebuffer)
        printf(" linear (0x%p)", mib.flat_frame_buffer);

      printf("\n");
    };

    mode_ptr++;
  };
}

vesa_mode* vesa_mode::find_mode(word xres, word yres,
  byte bits_per_pixel, bool exact_match)
{
  vesa_vbe_info_block vib = { 0 };
  bool avail = get_vbe_info_block(&vib);

  if (!avail)
    return NULL;

  word far* mode_ptr = vib.video_mode_list;
  vesa_mode_info_block mib = { 0 };

  word far* inexact_mode_match = NULL;
  word inexact_xres = 0, inexact_yres = 0;

  while (*mode_ptr != (word)-1)
  {
    if (get_mode_info_block(&mib, *mode_ptr))
    {
      if (!mib.modeflag.graph_text)
        continue;

      if (exact_match &&
        mib.xres == xres &&
        mib.yres == yres &&
        mib.bits_per_pixel == bits_per_pixel)
      {
        word mode = *mode_ptr;
        if (get_mode_info_block(&mib, mode))
          return create_mode(&mib, mode);
      }

      if (!exact_match &&
        mib.xres >= xres &&
        mib.yres >= yres &&
        mib.bits_per_pixel == bits_per_pixel)
      {
        if (inexact_mode_match == NULL ||
          (mib.xres < inexact_xres && mib.yres < inexact_yres))
        {
          inexact_xres = mib.xres;
          inexact_yres = mib.yres;
          inexact_mode_match = mode_ptr;
        }
      }
    }

    mode_ptr++;
  };

  if (!exact_match &&
    inexact_mode_match != NULL)
  {
    word mode = *inexact_mode_match;
    if (get_mode_info_block(&mib, mode))
      return create_mode(&mib, mode);
  }

  return NULL;
}

vesa_mode* vesa_mode::create_mode(struct vesa_mode_info_block far* mib, word mode)
{
  switch (mib->bits_per_pixel)
  {
  case 4: return new vesa_mode_4bit(mib, mode);
  case 8: return new vesa_mode_8bit(mib, mode);
  case 15:
  case 16: return new vesa_mode_16bit(mib, mode);
  case 24: return new vesa_mode_24bit(mib, mode);
  default:
    assert(false); // invalid bits per pixel value
    break;
  };

  return NULL;
}

// when windowing function is not available, call VBE function 05h
void far alternate_setframe()
{
  asm {
    mov bx, 0h
    int 10h
  };
}

vesa_mode::vesa_mode(vesa_mode_info_block* mib, word _mode)
  :mode(_mode),
  segment(mib->seg1)
{
  xres = mib->xres;
  yres = mib->yres;

  bytes_per_line = mib->bytes_per_line;
  bits_per_pixel = mib->bits_per_pixel;
  granularity_kb = mib->granularity_kb << 10;

  if (granularity_kb == 0)
    granularity_kb = mib->granularity_kb;

  has_linear_framebuffer = mib->modeflag.linear_framebuffer;

  start = has_linear_framebuffer
    ? mib->flat_frame_buffer
    : (byte far*)farpointer(segment, 0);
  last_page = (word)-1;

  if ((setframe = mib->setframe) == NULL)
  {
    setframe = (void(far*)())(alternate_setframe);
  }
}

vesa_mode::~vesa_mode()
{
  delete saved_state_buffer;
  saved_state_buffer = NULL;
}

void vesa_mode::on()
{
  _CX = mode;
  asm {
    mov ax, 0x4f02
    int 10h
  };

  is_on = (bool)(_AL == 0x4f);
}

void vesa_mode::off()
{
  asm {
    mov ax, 0x3
    int 10h
  };

  is_on = false;
}

byte far* vesa_mode::get_framebuffer_ptr(word x, word y)
{
  if (!has_linear_framebuffer)
    return NULL;

  return start + y * xres + x * (bits_per_pixel >> 3);
}

void vesa_mode::getpix(word x, word y, byte* pixel)
{
  memcpy(pixel, get_buffer_pos(x, y),
    bits_per_pixel >> 3);
}

#pragma argsused
void vesa_mode::set_palette(const palette256 palette)
{
  assert(false); // invalid mode that doesn't use a palette!
}

void vesa_mode::setpix(word x, word y, const byte* pixel)
{
  memcpy(get_buffer_pos(x, y), pixel,
    bits_per_pixel >> 3);
}

void vesa_mode::setline(word x, word y,
  word length_in_bytes, const byte* line)
{
  memcpy(get_buffer_pos(x, y), line,
    (bits_per_pixel >> 3) * length_in_bytes);
}

void vesa_mode::save_mode()
{
  word mem;
  asm {
    mov ax, 0x4f04 // 04h: save/restore video mode state
    mov dl, 0x0    // 00h: return state buffer size
    mov cx, 0xf    // bits 0-3: all states
    int 0x10
    mov mem, bx    // returns size of 64-byte blocks of buffer
  };

  byte far* state_buffer = (saved_state_buffer = new byte[mem << 6]);

  asm {
    mov ax, 0x4f04 // 04h: save/restore video mode state
    mov dl, 0x1    // 01h: save video mode state
    mov cx, 0xf    // bits 0-3: all states
    les bx, state_buffer
    int 0x10
  };
}

void vesa_mode::restore_mode()
{
  byte far* state_buffer = saved_state_buffer;
  asm {
    mov ax, 0x4f04 // 04h: save/restore video mode state
    mov dl, 0x2    // 02h: restore video mode state
    mov cx, 0xf    // bits 0-3: all states
    les bx, state_buffer
    int 0x10
  };

  delete saved_state_buffer;
  saved_state_buffer = NULL;
}

byte far* vesa_mode::get_buffer_pos(word x, word y)
{
  if (has_linear_framebuffer)
  {
    return get_framebuffer_ptr(x, y);
  }
  else
  {
    ulong offset = (ulong)y * bytes_per_line + x * (bits_per_pixel >> 3);
    word page = (word)(offset / granularity_kb);
    set_page(page);
    return start + (offset % granularity_kb);
  }
}

void vesa_mode::set_page(word page)
{
  assert(has_linear_framebuffer == false);

  if (page != last_page)
  {
    void (far * _setframe)() = setframe;
    asm {
      mov ax, 0x4f05
      mov bx, 0x0
      mov dx, page
    };

    _setframe();
    last_page = page;
  }
}

void vesa_mode_8bit::set_palette(const palette256 palette)
{
  set_vga_palette(palette);
}

#pragma argsused
void vesa_mode_4bit::setpix(word x, word y, const byte* pixel)
{
  assert(false); // not implemented!
}

#pragma argsused
void vesa_mode_4bit::setline(word x, word y, word length, const byte* line)
{
  assert(false); // not implemented!
}

void vesa_mode_4bit::set_palette(const palette256 palette)
{
  set_vga_palette(palette);
}
