/*
   altpng - a versatile png reading and writing library
   Copyright (c) 2003,2004,2021 Michael Fink

*/
/*! \file altpng.h

   this is the API header file for altpng

*/

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/* file util.h

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
   ape_ok=0,         /*!< all is ok */
   ape_error=-1,     /*!< general error, not further specified */
   ape_pointer=-2,   /*!< pointer error; an invalid pointer was passed */
   ape_memory=-3,    /*!< memory error; no memory could be allocated */
   ape_invalid_png=-4,  /*!< decoder encountered invalid or nonconformant PNG
                          file */
   ape_stream=-5,    /*!< error while reading/writing stream */
   ape_chunk_crc=-6, /*!< error in the chunk crc */
   ape_zlib=-7,      /*!< error during zlib processing */
   ape_scanline_nextchunk=-8, /*!< indicates that altpng_scanline_decode needs
                          a new chunk */
   ape_scanline_eos=-9, /*!< indicates that altpng_scanline_decode reached end
                          of stream */
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

/* file stream.h

   \brief Stream input / output interface

   Definitions and functions for a stream that is used throughout altpng to
   manage file input / output.

*/

#ifndef altpng_stream_h_
#define altpng_stream_h_



/* Typedefs */

/*! \brief Seek function pointer type */
typedef int (*altpng_func_seek)(void* data, long offset, int origin);

/*! \brief Read function pointer type */
typedef size_t (*altpng_func_read)(unsigned char* buffer,
   size_t size, size_t count, void* data);

/*! \brief Write function pointer type */
typedef size_t (*altpng_func_write)(const unsigned char* buffer,
   size_t size, size_t count, void* data);


/* Structs */

/*! \brief Stream data */
/*! Methods that can be used with altpng_image_info:
    \sa altpng_stream_init()<br>
      altpng_stream_finish()<br>
      altpng_stream_skip()<br>
      altpng_stream_sync()<br>
      altpng_stream_read()<br>
      altpng_stream_read16()<br>
      altpng_stream_read32()<br>
      altpng_stream_write()<br>
      altpng_stream_write16()<br>
      altpng_stream_write32()
*/
typedef struct altpng_stream
{
   /*! \brief indicates stream type; 1:input stream, 0:output stream */
   int input;

   /*! \brief input stream buffer */
   unsigned char* buffer;

   /*! \brief end of buffer */
   unsigned char* bufend;

   /*! \brief current buffer pointer */
   unsigned char* bufptr;

   /*! \brief user-defined data */
   void* user_data;

   /* user functions for stream input/output */

   /*! \brief read function pointer */
   altpng_func_read read;
   /*! \brief seek function pointer */
   altpng_func_seek seek;
   /*! \brief write function pointer */
   altpng_func_write write;

} altpng_stream;


/* Functions */

/*! \brief Inits stream */
int altpng_stream_init(altpng_stream* stream, int input, void* user_data);

/*! \brief Clears up stream */
void altpng_stream_finish(altpng_stream* stream);


/* input stream functions */

/*! \brief Skips nuber of bytes in the stream */
int altpng_stream_skip(altpng_stream* stream, int offset);

/*! \brief Synchronizes stream buffer content with stream, e.g. by reading in
    some bytes */
int altpng_stream_sync(altpng_stream* stream);

/*! \brief Reads a number of bytes from the input stream */
int altpng_stream_read(altpng_stream* stream, unsigned char* data, int length);

/*! \brief Reads a 16 bit integer from the input stream */
int altpng_stream_read16(altpng_stream* stream, altpng_uint16* num);

/*! \brief Reads a 32 bit integer from the input stream */
int altpng_stream_read32(altpng_stream* stream, altpng_uint32* num);


/* output stream functions */

/*! \brief Writes a number of bytes to the output stream */
int altpng_stream_write(altpng_stream* stream, unsigned char* data, int length);

/*! \brief Writes a 16 bit integer to the output stream */
int altpng_stream_write16(altpng_stream* stream, altpng_uint16 num);

/*! \brief Writes a 32 bit integer to the output stream */
int altpng_stream_write32(altpng_stream* stream, altpng_uint32 num);


#endif

/* file chunk.h

   \brief Functions to read and write PNG chunks.

*/

#ifndef altpng_chunk_h_
#define altpng_chunk_h_



/* structs */

/*! \brief Struct that holds PNG chunk data. */
/*! Methods that can be used with altpng_chunk:
    \sa altpng_chunk_init()<br>
      altpng_chunk_finish()<br>
      altpng_chunk_set_data()<br>
      altpng_chunk_load()<br>
      altpng_chunk_save()
*/
typedef struct altpng_chunk
{
   /*! \brief number of bytes in the chunk's data field */
   unsigned int length;

   /*! \brief chunk type */
   char name[5];

   /*! \brief pointer to chunk data */
   void* data;

   /*! \brief crc of the chunk (read from file) */
   unsigned int crc32;

   /*! \brief crc of the chunk (calculated from data) */
   unsigned int crc32_calc;

   /* chunk property flags */

   /*! \brief indicates if chunk is an ancilliary one; 0: critical; 1: ancilliary */
   unsigned int ancilliary;

   /*! \brief indicates if chunk is a private one; 0: public; 1: private */
   unsigned int private_bit;

   /*! \brief reserved bit; must be 0 for a valid PNG chunk */
   unsigned int reserved_bit;

   /*! \brief indicates if chunk is safe to copy; 0: unsafe; 1: safe to copy */
   unsigned int safe_to_copy;

} altpng_chunk;


/* functions */

/*! \brief Inits a chunk object */
void altpng_chunk_init(altpng_chunk* chunk);

/*! \brief Cleans up chunk object */
void altpng_chunk_finish(altpng_chunk* chunk);

/*! \brief Copies data to the chunk object */
int altpng_chunk_set_data(altpng_chunk* chunk, void* data, unsigned int length);

/*! \brief Loads a chunk from stream */
int altpng_chunk_load(altpng_chunk* chunk, altpng_stream* stream);

/*! \brief Saves a chunk to stream */
int altpng_chunk_save(altpng_chunk* chunk, altpng_stream* stream);


#endif

/* file image.h

   \brief Image info function.

*/

#ifndef altpng_image_h_
#define altpng_image_h_



/* structs */

/*! \brief a struct that holds PNG image info */
/*! Methods that can be used with altpng_image_info:
    \sa altpng_image_info_parse()
*/
typedef struct altpng_image_info
{
   /*! \brief image width */
   altpng_uint32 width;

   /*! \brief image height */
   altpng_uint32 height;

   /*! \brief image bit depth */
   altpng_uint8 bit_depth;

   /*! \brief color type
       0: grayscale image
       2: RGB truecolor image
       3: palette indexed image
       4: grayscale with alpha image
       6: RGBA truecolor with alpha image
   */
   altpng_uint8 color_type;

   /*! \brief compression method
       0: deflate/inflate compression method
   */
   altpng_uint8 compression_method;

   /*! \brief filter method
       0: adaptive filtering with 5 different filters
   */
   altpng_uint8 filter_method;

   /*! \brief interlace method
       0: no interlace
       1: Adam7 interlace
   */
   altpng_uint8 interlace_method;

   /*! \brief indicates number of bytes forming a color sample */
   int bytes_per_color;

   /*! \brief number of palette indices */
   unsigned int palette_indices;

   /*! \brief palette */
   altpng_uint8* palette;

} altpng_image_info;


/* Functions */

void altpng_image_info_init(altpng_image_info* image_info);
void altpng_image_info_finish(altpng_image_info* image_info);

/*! \brief Parses image info from the IHDR chunk */
int altpng_image_info_parse(altpng_image_info* image_info, altpng_chunk* chunk);

/*! \brief Extracts palette from PLTE chunk */
int altpng_image_info_get_palette(altpng_image_info* image_info, altpng_chunk* chunk);


#endif

/* file scanline.h

   \brief scanline reading and decoding functions

*/

#ifndef altpng_scanline_h_
#define altpng_scanline_h_



/* constants */

/*! size of the raw scanline buffer */
static const unsigned int altpng_scanline_rawbufsize = 2048;


/* forward declarations */

/*! pointer to zlib z_stream_s struct (forward declaration) */
typedef struct z_stream_s* z_stream_ptr;


/* structs */

/*! \brief holds a scanline and all associated data */
typedef struct altpng_scanline
{
   /*! scanline data */
   char* scanline;

   /*! number of color samples in scanline; may vary when image is
       interlaced */
   int color_samples;

   /*! filter type of scanline */
   unsigned int filter_type;

   /*! indicates number of current interlace pass */
   int interlace_pass;


   /*! unfiltered last scanline */
   char* last_scanline;


   /* decoding status data */

   /*! zlib compressed data stream */
   z_stream_ptr zstream;

   /*! indicates if decoder is at scanline start */
   int line_start;

   /*! indicates number of bytes in scanline that still have to be filled */
   int sl_avail;

   /*! size of scanline buffer */
   int sl_size;

   /*! size of last scanline data */
   int lsl_size;

   /*! indicates that the zlib stream ended */
   int stream_end;

   /*! bytes available in raw buffer */
   int raw_count;

   /*! raw buffer, always altpng_scanline_rawbufsize long */
   char* raw_buffer;

} altpng_scanline;


/* functions */

/*! inits scanline */
int altpng_scanline_init(altpng_scanline* scanline);

/*! cleans up scanline struct */
void altpng_scanline_finish(altpng_scanline* scanline);

/*! tries to decode one scanline using the chunk passed (must be an IDAT)
    when the next chunk is needed, -1 is returned;
    when scanline was completely decoded, 0 is returned */
int altpng_scanline_decode(altpng_scanline* scanline, altpng_image_info* info,
   altpng_chunk* chunk);

/*! decodes filtering of the current scanline */
int altpng_scanline_filter_decode(altpng_scanline* scanline,
   altpng_image_info* info);

#endif

/* file decoder.h

   \brief high-level functions for decoding a PNG image

*/

#ifndef altpng_decoder_h_
#define altpng_decoder_h_



/* typedefs */

/*! \brief forward pointer declaration*/
typedef struct altpng_decoder* altpng_decoder_ptr;

/*! \brief chunk processing callback function pointer type */
typedef int (*altpng_func_process_chunk)(altpng_decoder_ptr decoder,
   altpng_chunk* chunk);

/*! \brief scanline processing callback function pointer type */
typedef int (*altpng_func_process_scanline)(altpng_decoder_ptr decoder,
   altpng_scanline* scanline);


/* structs */

/*! \brief Struct that holds PNG decoding parameter */
typedef struct altpng_decoder
{
   /*! \brief info of current png image */
   altpng_image_info image_info;

   /*! \brief chunk processing callback */
   altpng_func_process_chunk process_chunk;

   /*! \brief scanline processing callback */
   altpng_func_process_scanline process_scanline;

   /*! \brief indicates if chunk data is deallocated after processing the chunk;
       default: 0 */
   int dealloc_data;

   /*! \brief list of chunks */
   altpng_chunk** chunklist;

   /*! \brief entries in chunklist */
   int listentries;

   /*! \brief size of chunklist array */
   int listsize;

   /*! \brief pointer to current chunk */
//   altpng_chunk* current_chunk;

   /*! \brief pointer to first IDAT chunk */
//   altpng_chunk* first_idat;

   /*! \brief file stream */
   altpng_stream* stream;

   /*! \brief user data pointer */
   void* user_data;

} altpng_decoder;


/* Functions */

/*! \brief Inits the decoder */
void altpng_decoder_init(altpng_decoder* decoder, altpng_stream* stream,
   void* user_data);

/*! \brief Finishes the decoder */
void altpng_decoder_finish(altpng_decoder* decoder);

/*! \brief Decodes header of PNG image */
int altpng_decoder_header(altpng_decoder* decoder);

/*! \brief Starts decoding of PNG image in stream */
int altpng_decoder_decode(altpng_decoder* decoder);


#endif

/* file crc32.h

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

#ifdef __cplusplus
}
#endif

