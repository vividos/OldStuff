/*
   altpng - a versatile png reading and writing library
   Copyright (c) 2003,2004,2021 Michael Fink

*/
/*! \file scanline.h

   \brief scanline reading and decoding functions

*/

#ifndef altpng_scanline_h_
#define altpng_scanline_h_

#include "util.h"
#include "chunk.h"
#include "image.h"


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
