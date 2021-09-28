/*
   altpng - a versatile png reading and writing library
   Copyright (c) 2003,2004,2021 Michael Fink

*/
/*! \file util.h

   \brief Some useful typedefs, macros and functions.

*/

#ifndef altpng_util_h_
#define altpng_util_h_


/*! \brief Returns version string */
const char* altpng_version_get();


/* Basic data types */

/*! \brief unsigned integer with at least 8 bit */
typedef unsigned char altpng_uint8;

/*! \brief unsigned integer with at least 16 bit */
typedef unsigned short altpng_uint16;

/*! \brief unsigned integer with at least 32 bit */
typedef unsigned int altpng_uint32;


/*! \brief Error enumeration */
typedef enum altpng_error
{
   ape_ok = 0,         /*!< all is ok */
   ape_error = -1,     /*!< general error, not further specified */
   ape_pointer = -2,   /*!< pointer error; an invalid pointer was passed */
   ape_memory = -3,    /*!< memory error; no memory could be allocated */
   /*! decoder encountered invalid or nonconformant PNG file */
   ape_invalid_png = -4,
   ape_stream = -5,    /*!< error while reading/writing stream */
   ape_chunk_crc = -6, /*!< error in the chunk crc */
   ape_zlib = -7,      /*!< error during zlib processing */
   /*! indicates that altpng_scanline_decode needs a new chunk */
   ape_scanline_nextchunk = -8,
   /*! indicates that altpng_scanline_decode reached end of stream */
   ape_scanline_eos = -9,
} altpng_error;


/* Endian conversion */
#ifdef WORDS_BIGENDIAN
# define altpng_endian_convert16(x) (x)
# define altpng_endian_convert32(x) (x)
#else
   /*! \brief converts 16-bit from host-order to network-order */
#define altpng_endian_convert16(x) ( (((x)&0x00ff)<<8) | (((x)&0xff00)>>8) )

/*! \brief converts 32-bit from host-order to network-order */
#define altpng_endian_convert32(x) \
      ( altpng_endian_convert16(((x)&0xffff0000)>>16) | \
        altpng_endian_convert16((x)&0x0000ffff)<<16 )
#endif


/*! \brief Checks for the PNG file signature */
int altpng_check_signature(unsigned char* buffer);

#endif
