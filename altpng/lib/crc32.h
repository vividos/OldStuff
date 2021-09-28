/*
   altpng - a versatile png reading and writing library
   Copyright (c) 2003,2004,2021 Michael Fink

*/
/*! \file crc32.h

   \brief Functions for calculating a 32 bit cyclic redundancy check value

   Function implementations were taken from the PNG specification.

*/

#ifndef altpng_crc32_h_
#define altpng_crc32_h_


/*! \brief initializes CRC table */
void altpng_crc32_init();

/*! \brief calculates CRC for the bytes in the buffer */
unsigned long altpng_crc32_crc(unsigned char* buf, int len);

/*! \brief updates a running CRC */
unsigned long altpng_crc32_update(unsigned long crc, unsigned char* buf,
   int len);


#endif
