/*
   altpng - a versatile png reading and writing library
   Copyright (c) 2003,2004,2021 Michael Fink

*/
/*! \file chunk.c

   \brief Chunk decoding and creating functions.

*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include "chunk.h"
#include "crc32.h"

/*! Inits chunk and sets all values to default; "data" member isn't allocated
    yet.
    \param chunk chunk object
*/
void altpng_chunk_init(altpng_chunk* chunk)
{
   chunk->length = 0;
   chunk->name[4] = 0;
   chunk->data = NULL;
   chunk->crc32 = 0xffffffff;
}

/*! Cleans up chunk and frees chunk data if set.
    \param chunk chunk object
*/
void altpng_chunk_finish(altpng_chunk* chunk)
{
   if (chunk->data != NULL)
   {
      free(chunk->data);
      chunk->data = NULL;
   }
}

/*! Sets data from given pointer; the memory is reallocated if needed.
    \param chunk chunk object
    \param data pointer to data
    \param length length in bytes of data to set
    \return ape_memory if no memory was available to reallocate
*/
int altpng_chunk_set_data(altpng_chunk* chunk, void* data, unsigned int length)
{
   /* alloc memory if needed */
   void* new_data = realloc(chunk->data, length);
   if (new_data == NULL)
      return ape_memory;

   chunk->data = new_data;

   /* copy data to chunk */
   memcpy(chunk->data, data, length);
   return ape_ok;
}

/*! \brief Calculates CRC for a given chunk. CRC is calculated for the chunk
    name bytes and the chunk data. The crc is stored in crc32_calc then.
    \param chunk chunk object
*/
void _altpng_chunk_calc_crc(altpng_chunk* chunk)
{
   /* calculate chunk crc */
   unsigned int crc32;

   crc32 = altpng_crc32_crc((unsigned char*)(chunk->name), 4);

   if (chunk->length > 0)
   {
      crc32 = altpng_crc32_update(crc32 ^ 0xffffffffL,
         (unsigned char*)(chunk->data), chunk->length);
      crc32 ^= 0xffffffffL;
   }
   chunk->crc32_calc = crc32;
}

/*! Loads chunk from given stream object.
    \param chunk chunk object
    \param stream stream object
    \return ape_pointer if a passed pointer was invalid, ape_stream if a
            stream error occured or ape_chunk_crc if calculated CRC didn't
            match.
*/
int altpng_chunk_load(altpng_chunk* chunk, altpng_stream* stream)
{
   int ret;
   void* new_data;

   if (chunk == NULL || stream == NULL)
      return ape_pointer;

   /* read chunk data length */
   ret = altpng_stream_read32(stream, &chunk->length);
   if (ret < 0) return ret;

   /* read chunk name */
   ret = altpng_stream_read(stream, chunk->name, 4);
   if (ret < 0) return ret;

   if (chunk->length > 0)
   {
      /* read data */
      new_data = realloc(chunk->data, chunk->length);
      if (new_data == NULL)
         return ape_memory;

      chunk->data = new_data;

      ret = altpng_stream_read(stream, chunk->data, chunk->length);
      if (ret < 0) return ret;
   }

   /* read crc */
   ret = altpng_stream_read32(stream, &chunk->crc32);
   if (ret < 0) return ret;

   /* calculate crc */
   _altpng_chunk_calc_crc(chunk);

   /* determine chunk property flags */
   chunk->ancilliary = (chunk->name[0] >> 5) & 1;
   chunk->private_bit = (chunk->name[1] >> 5) & 1;
   chunk->reserved_bit = (chunk->name[2] >> 5) & 1;
   chunk->safe_to_copy = (chunk->name[3] >> 5) & 1;

   /* check crc */
   if (chunk->crc32 != chunk->crc32_calc)
      return ape_chunk_crc;

   return ape_ok;
}

/*! Saves chunk using the stream object. The stream must be initialized as
    output stream.
    \param chunk chunk object
    \param stream stream object
    \return ape_pointer if a passed pointer was invalid or ape_stream if a
            stream error occured.
*/
int altpng_chunk_save(altpng_chunk* chunk, altpng_stream* stream)
{
   int ret;

   if (chunk == NULL || stream == NULL)
      return ape_pointer;

   /* write chunk data length */
   ret = altpng_stream_write32(stream, chunk->length);
   if (ret < 0) return ret;

   /* write chunk name */
   ret = altpng_stream_write(stream, chunk->name, 4);
   if (ret < 0) return ret;

   if (chunk->length > 0)
   {
      /* write data */
      ret = altpng_stream_write(stream, chunk->data, chunk->length);
      if (ret < 0) return ret;
   }

   /* calculate crc */
   _altpng_chunk_calc_crc(chunk);

   /* write crc */
   return altpng_stream_write32(stream, chunk->crc32_calc);
}
