/*
   altpng - a versatile png reading and writing library
   Copyright (c) 2003,2004,2021 Michael Fink

*/
/*! \file decoder.h

   \brief high-level functions for decoding a PNG image

*/

#ifndef altpng_decoder_h_
#define altpng_decoder_h_

#include "chunk.h"
#include "scanline.h"
#include "image.h"


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
   //altpng_chunk* current_chunk;

   /*! \brief pointer to first IDAT chunk */
   //altpng_chunk* first_idat;

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
