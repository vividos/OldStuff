// lzwdecod.cpp - LZW decoder
// (c) 1996, 2020 Michael Fink
// https://github.com/vividos/OldStuff/

// Original header:
// DECODE.C - An LZW decoder for GIF
// Copyright (C) 1987, by Steven A. Bennett
//
// Permission is given by the author to freely redistribute and include
// this code in any program as long as this credit is given where due.
//
// In accordance with the above, I want to credit Steve Wilhite who wrote
// the code which this is heavily inspired by...
//
// GIF and 'Graphics Interchange Format' are trademarks (tm) of
// Compuserve, Incorporated, an H&R Block Company.
//
// Release Notes: This file contains a decoder routine for GIF images
// which is similar, structurally, to the original routine by Steve Wilhite.
// It is, however, somewhat noticably faster in most cases.

#include "lzwdecod.h"
#include "defs.h"
#include <stdio.h>
#include <mem.h>
#include <assert.h>

const size_t c_pixel_buffer_size = 256;

// bitmask to mask the first N bits of a WORD value
unsigned long code_mask[13] =
{
  0x0000L, 0x0001L, 0x0003L, 0x0007L, 0x000FL,
  0x001FL, 0x003FL, 0x007FL, 0x00FFL, 0x01FFL,
  0x03FFL, 0x07FFL, 0x0FFFL
};

lzw_decoder::lzw_decoder(FILE* _fd, byte _compression_size)
  :fd(_fd),
  compression_size(_compression_size),
  navail_bytes(0),
  pbytes(byte_buffer),
  nbits_left(0),
  bits_avail(0)
{
  assert(_compression_size >= 2 && _compression_size <= 9);

  // Initializes the decoder for reading a new image.
  memset(&byte_buffer, 0, sizeof(byte_buffer));

  current_bit_size = compression_size + 1;
  clear_value = 1 << compression_size;
  eoi_value = clear_value + 1;
  top_slot = 1 << current_bit_size;
  slot = new_codes = eoi_value + 1;

  stack = new byte[4096 + 1];
  suffix = new byte[4096 + 1];
  prefix = new word[4096 + 1];

  // Initialize in case they forgot to put in a clear code.
  // (This shouldn't happen, but we'll try and decode it anyway...)
  oc = fc = 0;

  // Set up the stack pointer
  sp = stack;

  pixel_buffer = new byte[c_pixel_buffer_size];
  pixel_buffer_avail = 0;
}

lzw_decoder::~lzw_decoder()
{
  delete stack;
  delete suffix;
  delete prefix;
  delete pixel_buffer;
}

void lzw_decoder::get_block(byte* buffer, word length)
{
  if (pixel_buffer_avail > 0)
  {
    word copy_size = pixel_buffer_avail < length ? pixel_buffer_avail : length;
    memcpy(buffer, pixel_buffer, copy_size);
    length -= copy_size;
    buffer += copy_size;
    pixel_buffer_avail -= copy_size;
  }

  // This is the main loop. For each code we get we pass through the
  // linked list of prefix codes, pushing the corresponding "character" for
  // each code onto the stack. When the list reaches a single "character"
  // we push that on the stack too, and then start unstacking each
  // character for output in the correct order. Special handling is
  // included for the clear code, and the whole thing ends when we get
  // an ending code.
  int c;
  word count = 0;
  if (count < length)
  {
    while ((c = get_next_code()) != eoi_value)
    {
      // If the code is a clear code, reinitialize all necessary items.
      if (c == clear_value)
      {
        current_bit_size = compression_size + 1;
        slot = new_codes;
        top_slot = 1 << current_bit_size;

        // Continue reading codes until we get a non-clear code
        // (Another unlikely, but possible case...)
        while ((c = get_next_code()) == clear_value);

        //  If we get an ending code immediately after a clear code
        // (Yet another unlikely case), then break out of the loop.
        if (c == eoi_value)
          break;

        // Finally, if the code is beyond the range of already set codes,
        // (This one had better NOT happen...  I have no idea what will
        // result from this, but I doubt it will look good...) then set it
        // to color zero.
        if (c >= slot)
          c = 0;

        oc = fc = c;

        // And let us not forget to put the char into the buffer...
        if (count < length)
        {
          *buffer++ = c;
          count++;
        }
        else
        {
          assert(pixel_buffer_avail < c_pixel_buffer_size);
          pixel_buffer[pixel_buffer_avail++] = c;
        }
      }
      else
      {
        // In this case, it's not a clear code or an ending code, so
        // it must be a code code... So we can now decode the code into
        // a stack of character codes. (Clear as mud, right ? )
        int code = c;

        // Here we go again with one of those off chances...  If, on the
        // off chance, the code we got is beyond the range of those already
        // set up (Another thing which had better NOT happen...) we trick
        // the decoder into thinking it actually got the last code read.
        // (Hmmn... I'm not sure why this works...  But it does...)
        if (code >= slot)
        {
          code = oc;
          *sp++ = fc;
        }

        // Here we scan back along the linked list of prefixes, pushing
        // helpless characters (ie.suffixes) onto the stack as we do so.
        while (code >= new_codes)
        {
          *sp++ = suffix[code];
          code = prefix[code];
        }

        // Push the last character on the stack, and set up the new
        // prefix and suffix, and if the required slot number is greater
        // than that allowed by the current bit size, increase the bit
        // size. (NOTE - If we are all full, we *don't* save the new
        // suffix and prefix...  I'm not certain if this is correct...
        // it might be more proper to overwrite the last code...
        *sp++ = code;
        if (slot < top_slot)
        {
          suffix[slot] = fc = code;
          prefix[slot++] = oc;
          oc = c;
        }

        if (slot >= top_slot && current_bit_size < 12)
        {
          top_slot <<= 1;
          ++current_bit_size;
        }

        // Now that we've pushed the decoded string (in reverse order)
        // onto the stack, lets pop it off and put it into our decode
        // buffer... And when the decode buffer is full, write another
        // line...
        while (sp > stack)
        {
          --sp;

          if (count < length)
          {
            *buffer++ = *sp;
            count++;
          }
          else
          {
            assert(pixel_buffer_avail < c_pixel_buffer_size);
            pixel_buffer[pixel_buffer_avail++] = *sp;
          }
        }
      }

      if (count >= length)
        break;
    };
  }
}

byte lzw_decoder::get_raw_byte()
{
  if (navail_bytes <= 0)
  {
    // Out of bytes in current block, so read next block
    pbytes = byte_buffer;
    navail_bytes = fgetc(fd);
    if (navail_bytes == EOF)
    {
      //error = true;
      return 0;
    }

    assert(navail_bytes < sizeof(byte_buffer));

    if (navail_bytes)
      fread(byte_buffer, 1, navail_bytes, fd);
  }

  byte raw = *pbytes++;
  --navail_bytes;

  return raw;
}

int lzw_decoder::get_next_code()
{
  if (!nbits_left)
  {
    bits_avail = get_raw_byte();
    nbits_left = 8;
  }

  unsigned long ret = bits_avail >> (8 - nbits_left);
  while (current_bit_size > nbits_left)
  {
    bits_avail = get_raw_byte();
    ret |= bits_avail << nbits_left;
    nbits_left += 8;
  }

  nbits_left -= current_bit_size;

  assert(current_bit_size < 13);
  ret &= code_mask[current_bit_size];
  return (int)ret;
}
