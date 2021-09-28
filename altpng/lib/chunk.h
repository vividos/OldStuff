/*
   altpng - a versatile png reading and writing library
   Copyright (c) 2003,2004,2021 Michael Fink

*/
/*! \file chunk.h

   \brief Functions to read and write PNG chunks.

*/

#ifndef altpng_chunk_h_
#define altpng_chunk_h_

#include "util.h"
#include "stream.h"


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
