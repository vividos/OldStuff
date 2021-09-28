/*
   altpng - a versatile png reading and writing library
   Copyright (c) 2003,2004,2021 Michael Fink

*/
/*! \file crc32.c

   \brief crc32 implementation

   The following CRC32 code is taken from the PNG specification
   and slightly modified in terms of variable and function names.

   The following sample code represents a practical implementation
   of the CRC (Cyclic Redundancy Check) employed in PNG chunks. (See
   also ISO 3309 [ISO-3309] or ITU-T V.42 [ITU-T-V42] for a formal
   specification.)

*/

#include "crc32.h"

/*! \brief Table of CRCs of all 8-bit messages. */
static unsigned long altpng_crc32_table[256];

/*! \brief Flag: has the table been computed? Initially false. */
int altpng_crc32_table_computed = 0;

/*! Make the table for a fast CRC. */
void altpng_crc32_init()
{
   unsigned long c;
   int n, k;

   for (n = 0; n < 256; n++)
   {
      c = (unsigned long)n;
      for (k = 0; k < 8; k++)
      {
         if (c & 1)
            c = 0xedb88320L ^ (c >> 1);
         else
            c = c >> 1;
      }
      altpng_crc32_table[n] = c;
   }

   altpng_crc32_table_computed = 1;
}

/*! Update a running CRC with the bytes buf[0..len-1]--the CRC
    should be initialized to all 1's, and the transmitted value
    is the 1's complement of the final running CRC (see the
    crc() routine below)).

    \param crc running crc value calculated in a previous run of
               altpng_crc32_crc or altpng_crc32_update
    \param buf buffer with more bytes to calculate CRC from
    \param len number of bytes to use for CRC calculation, in bytes
    \return calculated CRC value
*/
unsigned long altpng_crc32_update(unsigned long crc, unsigned char* buf,
   int len)
{
   unsigned long c = crc;
   int n;

   if (!altpng_crc32_table_computed)
      altpng_crc32_init();
   for (n = 0; n < len; n++) {
      c = altpng_crc32_table[(c ^ buf[n]) & 0xff] ^ (c >> 8);
   }
   return c;
}

/*! Return the CRC of the bytes buf[0..len-1].

    \param buf buffer with bytes to calculate CRC from
    \param len number of bytes to use for CRC calculation, in bytes
    \return calculated CRC value
*/
unsigned long altpng_crc32_crc(unsigned char* buf, int len)
{
   return altpng_crc32_update(0xffffffffL, buf, len) ^ 0xffffffffL;
}
