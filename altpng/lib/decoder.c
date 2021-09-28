/*
   altpng - a versatile png reading and writing library
   Copyright (c) 2003,2004,2021 Michael Fink

*/
/*! \file decoder.c

   \brief Higher-level PNG decoding functionality

*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <string.h>
#include "decoder.h"
#include "util.h"

/*! Dummy chunk processing callback */
int altpng_callback_process_chunk(altpng_decoder* decoder,
   altpng_chunk* chunk)
{
   return 0;
}

/*! Dummy scanline processing callback */
int altpng_callback_process_scanline(altpng_decoder* decoder,
   altpng_scanline* scanline)
{
   return 0;
}

void altpng_decoder_init(altpng_decoder* decoder, altpng_stream* stream,
   void* user_data)
{
   int i;
   decoder->process_chunk = altpng_callback_process_chunk;
   decoder->process_scanline = altpng_callback_process_scanline;
   decoder->dealloc_data = 0;
   decoder->stream = stream;
   decoder->user_data = user_data;

   altpng_image_info_init(&decoder->image_info);

   /* set up chunk list */
   decoder->listsize = 16;
   decoder->chunklist = malloc(decoder->listsize * sizeof(altpng_chunk*));

   if (decoder->chunklist != NULL)
      for (i = 0; i < decoder->listsize; i++)
         decoder->chunklist[i] = NULL;
   decoder->listentries = 0;

   //decoder->current_chunk = NULL;
   //decoder->first_idat = NULL;
}

void altpng_decoder_finish(altpng_decoder* decoder)
{
   int i;
   if (decoder->chunklist != NULL)
   {
      /* free each chunk list entry */
      for (i = 0; i < decoder->listentries; i++)
      {
         altpng_chunk_finish(decoder->chunklist[i]);
         free(decoder->chunklist[i]);
         decoder->chunklist[i] = NULL;
      }

      free(decoder->chunklist);
   }

   altpng_image_info_finish(&decoder->image_info);
}

/*! \brief inserts chunk into chunk list */
void _altpng_decoder_insert_chunk(altpng_decoder* decoder,
   altpng_chunk* chunk)
{
   /* check if we have to realloc chunk list */
   if (decoder->listentries + 1 >= decoder->listsize)
   {
      /* todo */
   }

   /* insert chunk */
   decoder->chunklist[decoder->listentries++] = chunk;
   //decoder->current_chunk = chunk;
}

/*! \brief processes chunk */
int _altpng_decoder_process_chunk(altpng_decoder* decoder,
   altpng_chunk* chunk)
{
   int ret = ape_ok;

   /*! \todo check for more chunks */

   if (0 == strcmp("PLTE", chunk->name))
      ret = altpng_image_info_get_palette(&decoder->image_info, chunk);

   return ret;
}

/*! decodes the first headers, until an IDAT chunk occurs; image info is
    available in decode->image_info then */
int altpng_decoder_header(altpng_decoder* decoder)
{
   int ret;
   unsigned char header[8];
   altpng_chunk* chunk;

   /* read and check PNG header */
   ret = altpng_stream_read(decoder->stream, header, 8);
   if (ret < 0) return ret;

   ret = altpng_check_signature(header);
   if (ret < 0) return ret;

   /* read first chunk: must be an IHDR */
   chunk = malloc(sizeof(altpng_chunk));
   if (chunk == NULL)
      return ape_memory;

   altpng_chunk_init(chunk);

   do /* tricky try..catch solution */
   {
      ret = altpng_chunk_load(chunk, decoder->stream);
      if (ret < 0) break;

      /* check length of IHDR chunk, must be 13 */
      if (chunk->length != 13)
      {
         ret = ape_invalid_png;
         break;
      }

      /* copy header data to image_info struct */
      ret = altpng_image_info_parse(&decoder->image_info, chunk);
      if (ret < 0) break;

      /* call chunk callback */
      ret = decoder->process_chunk(decoder, chunk);
      if (ret < 0) break;

   } while (0);

   /* free IHDR chunk */
   altpng_chunk_finish(chunk);
   free(chunk);

   return ret;
}

int altpng_decoder_decode(altpng_decoder* decoder)
{
   int ret, exit;
   unsigned int line = 0;
   altpng_chunk chunk;
   altpng_scanline scanline;


   /* check if first IDAT chunk exists */
   if (decoder == NULL)
      return ape_pointer;

   ret = altpng_scanline_init(&scanline);
   if (ret != ape_ok)
      return ret;

   /* move through all chunks */
   exit = 0;
   line = 0;
   do
   {
      /* load chunk */
      altpng_chunk_init(&chunk);

      ret = altpng_chunk_load(&chunk, decoder->stream);
      if (ret < 0) break;

      /* process chunk */
      ret = _altpng_decoder_process_chunk(decoder, &chunk);
      if (ret < 0) break;

      /* call chunk callback */
      ret = decoder->process_chunk(decoder, &chunk);
      if (ret < 0) break;

      if (memcmp(chunk.name, "IEND", 4) == 0)
      {
         exit = 1;
         ret = chunk.length == 0 ? ape_ok : ape_invalid_png;
      }
      else
         if (memcmp(chunk.name, "IDAT", 4) == 0)
         {
            /* decode scanline until the next IDAT chunk is needed */
            do
            {
               /* decode scanline */
               ret = altpng_scanline_decode(&scanline, &decoder->image_info, &chunk);

               if (ret == ape_ok)
               {
                  int ret2;

                  line++;

                  /* call scanline callback */
                  ret2 = decoder->process_scanline(decoder, &scanline);
                  if (ret2 < 0)
                  {
                     ret = ret2;
                     exit = 1;
                     break;
                  }
               }

            } while (ret == ape_ok && ret != ape_scanline_nextchunk &&
               line < decoder->image_info.height);
         }

      /* decoded all scanlines? */
      if (line >= decoder->image_info.height)
         exit = 1;

      altpng_chunk_finish(&chunk);

   } while (!exit);

   altpng_scanline_finish(&scanline);

   return ret;
}
