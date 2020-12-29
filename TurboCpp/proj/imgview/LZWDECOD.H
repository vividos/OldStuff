// lzwdecod.h - LZW decoder
// (c) 1996, 2020 Michael Fink
// https://github.com/vividos/OldStuff/

#pragma once

#ifndef __lzwdecod_h_
#define __lzwdecod_h_

#include "defs.h"
#include <stdio.h>

// LZW (Lempel-Ziv-Welch) decoder for use in decoding GIF data blocks
// See: http://matthewflickinger.com/lab/whatsinagif/lzw_image_data.asp
class lzw_decoder
{
public:
  // ctor
  lzw_decoder(FILE* fd, byte size);
  // dtor
  ~lzw_decoder();

  // decodes and stores a block of data in the given buffer
  void get_block(byte* buffer, word length);

private:
  // gets next raw byte from data sub-block
  byte get_raw_byte();

  // gets the next code word
  int get_next_code();

private:
  // file to read bytes from
  FILE* fd;

  // LZW compression size, in bits
  byte compression_size;

  // value of 'clear' code that clears the table
  int clear_value;

  // value of 'end of information' code that ends decoding image data
  int eoi_value;

  // data sub-block reading variables

  // number of available bytes in byte_buffer
  int navail_bytes;

  // byte buffer, containing data of one sub-block
  byte byte_buffer[257];

  // pointer into the byte_buffer to the next raw byte to read
  byte* pbytes;

  // next code byte variables

  // current bit size of a code word to read
  int current_bit_size;

  // number of bits left in bits_avail
  int nbits_left;

  // contains remaining bits of current raw byte
  byte bits_avail;

  // LZW decoding variables

  // first available code in TODO
  int new_codes;

  // highest code value for current bit size of code words
  int top_slot;

  // last read code word
  int slot;

  int fc, oc;

  // stack for storing pixels
  byte* stack;

  // suffix table
  byte* suffix;
  // prefix linked list
  word* prefix;

  byte* sp;

  // buffer of pixels already decoded, but not requested by get_block()
  byte* pixel_buffer;

  // number of pixels available in pixel_buffer
  word pixel_buffer_avail;
};

#endif
