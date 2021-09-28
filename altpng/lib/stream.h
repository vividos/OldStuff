/*
   altpng - a versatile png reading and writing library
   Copyright (c) 2003,2004,2021 Michael Fink

*/
/*! \file stream.h

   \brief Stream input / output interface

   Definitions and functions for a stream that is used throughout altpng to
   manage file input / output.

*/

#ifndef altpng_stream_h_
#define altpng_stream_h_

#include "util.h"


/* Typedefs */

/*! \brief Seek function pointer type */
typedef int (*altpng_func_seek)(void* data, long offset, int origin);

/*! \brief Read function pointer type */
typedef size_t(*altpng_func_read)(unsigned char* buffer,
   size_t size, size_t count, void* data);

/*! \brief Write function pointer type */
typedef size_t(*altpng_func_write)(const unsigned char* buffer,
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
