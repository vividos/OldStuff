/*
   altpng - a versatile png reading and writing library
   Copyright (c) 2003,2004,2021 Michael Fink

*/
/*! \file stream.c

   \brief Stream read and write function implementation.

   altpng_stream is a generic customizable interface for reading and writing
   data bytes from or to a logical stream

*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include "stream.h"
#include "util.h"

/*! \brief Stream buffer size */
const int altpng_stream_buffer_size = 1024;

/*! Initializes stream object for input or output.
    \param stream stream object
    \param input set to 1 when input stream, 0 when output stream
    \param user_data user-specified data for use in read, seek and write
           functions
    \return ape_memory when no memory could be allocated for the internal
            buffer
*/
int altpng_stream_init(altpng_stream* stream, int input, void* user_data)
{
   stream->buffer = malloc(altpng_stream_buffer_size);
   if (stream->buffer == NULL) return ape_memory;

   stream->bufend = stream->buffer + altpng_stream_buffer_size;
   stream->bufptr = stream->bufend; /* forces resync on first read */
   stream->input = input;
   stream->user_data = user_data;
   stream->read = (altpng_func_read)fread;
   stream->seek = (altpng_func_seek)fseek;
   stream->write = (altpng_func_write)fwrite;
   return ape_ok;
}

void altpng_stream_finish(altpng_stream* stream)
{
   if (stream->buffer != NULL)
      free(stream->buffer);
   stream->buffer = stream->bufend = stream->bufptr = NULL;
}

int altpng_stream_skip(altpng_stream* stream, int offset)
{
   int remaining;

   if (stream == NULL || stream->bufptr == NULL || stream->bufend == NULL)
      return ape_pointer;

   remaining = stream->bufend - stream->bufptr;

   if (remaining > offset)
   {
      stream->bufptr += offset;
   }
   else
   {
      /* seek to position */
      stream->seek(stream->user_data, offset - remaining, SEEK_CUR);

      /* force resync */
      stream->bufend = stream->bufptr;

      /* sync stream with file */
      return altpng_stream_sync(stream);
   }

   return ape_ok;
}

int altpng_stream_sync(altpng_stream* stream)
{
   int ret, remaining = stream->bufend - stream->bufptr;

   if (stream == NULL || stream->bufptr == NULL || stream->bufend == NULL)
      return ape_pointer;

   if (remaining > 0)
      memmove(stream->buffer, stream->bufptr, remaining);

   /* fill buffer */
   ret = stream->read(stream->buffer + remaining,
      1, (altpng_stream_buffer_size - remaining), stream->user_data);

   if (ret < 0 || ret + remaining == 0)
      return ape_stream;

   /* set current buffer pointer to start */
   stream->bufptr = stream->buffer;

   return ape_ok;
}

int altpng_stream_read(altpng_stream* stream, unsigned char* data, int length)
{
   int size, ret;

   if (stream == NULL || stream->bufptr == NULL || stream->bufend == NULL ||
      data == NULL)
      return ape_pointer;

   /* first, check if we have to resync buffer */
   if (stream->bufend - stream->bufptr > length)
   {
      memmove(data, stream->bufptr, length);
      stream->bufptr += length;
      return ape_ok;
   }

   /* we have to resync */
   while (length > 0)
   {
      /* determine maximum size to read in into buffer */
      size = length > altpng_stream_buffer_size ?
         altpng_stream_buffer_size : length;

      if (stream->bufend - stream->bufptr < size)
      {
         ret = altpng_stream_sync(stream);
         if (ret < 0) return ret;
      }

      memmove(data, stream->buffer, size);

      data += size;
      length -= size;
      stream->bufptr += size;
   }
   return ape_ok;
}

int altpng_stream_read16(altpng_stream* stream, altpng_uint16* num)
{
   altpng_uint16 val;
   int ret;

   ret = altpng_stream_read(stream, (unsigned char*)&val, 2);
   if (ret < 0) return ret;

   *num = altpng_endian_convert16(val);
   return ape_ok;
}

int altpng_stream_read32(altpng_stream* stream, altpng_uint32* num)
{
   altpng_uint32 val;
   int ret;

   ret = altpng_stream_read(stream, (unsigned char*)&val, 4);
   if (ret < 0) return ret;

   *num = altpng_endian_convert32(val);
   return ape_ok;
}

int altpng_stream_write(altpng_stream* stream, unsigned char* data, int length)
{
   int ret = stream->write(data, 1, length, stream->user_data);
   return ret < 0 ? ape_stream : ape_ok;
}

int altpng_stream_write16(altpng_stream* stream, altpng_uint16 num)
{
   altpng_uint16 val = altpng_endian_convert16(num);
   return altpng_stream_write(stream, (unsigned char*)&val, 2);
}

int altpng_stream_write32(altpng_stream* stream, altpng_uint32 num)
{
   altpng_uint32 val = altpng_endian_convert32(num);
   return altpng_stream_write(stream, (unsigned char*)&val, 4);
}
