/*
   altpng - a versatile png reading and writing library
   Copyright (c) 2003,2004,2021 Michael Fink

*/
/*! \file scanline.c

   \brief scanline reading and decoding functions

*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <string.h>
#include "scanline.h"
#include <zlib.h>

int altpng_scanline_init(altpng_scanline* scanline)
{
   /* init zlib struct */
   scanline->zstream = malloc(sizeof(z_stream));
   if (scanline->zstream == NULL)
      return ape_memory;

   scanline->zstream->zalloc = Z_NULL;
   scanline->zstream->zfree = Z_NULL;
   scanline->zstream->opaque = Z_NULL;
   inflateInit(scanline->zstream);

   scanline->zstream->next_in = NULL;

   scanline->raw_buffer = malloc(altpng_scanline_rawbufsize);
   scanline->raw_count = 0;

   scanline->color_samples = 0;
   scanline->interlace_pass = 0;
   scanline->sl_avail = 1;
   scanline->line_start = 1;

   scanline->scanline = NULL;
   scanline->sl_size = 0;
   scanline->stream_end = 0;

   scanline->last_scanline = NULL;
   scanline->lsl_size = 0;

   return ape_ok;
}

void altpng_scanline_finish(altpng_scanline* scanline)
{
   if (scanline->zstream != NULL)
   {
      inflateEnd(scanline->zstream);

      if (scanline->zstream != NULL)
         free(scanline->zstream);
   }

   if (scanline->raw_buffer != NULL)
      free(scanline->raw_buffer);

   if (scanline->scanline != NULL)
      free(scanline->scanline);

   if (scanline->last_scanline != NULL)
      free(scanline->last_scanline);

   scanline->zstream = NULL;
   scanline->raw_buffer = NULL;
   scanline->scanline = NULL;
   scanline->last_scanline = NULL;
}

int altpng_scanline_decode(altpng_scanline* scanline, altpng_image_info* info,
   altpng_chunk* chunk)
{
   /* decodes IDAT chunk data to scanline, until we are out of data or
      we have one complete scanline */

   int err;
   int size;
   char* source, * target;
   void* new_data;

   /* calculate length of a raw line */
   /* todo take adam7 interlacing into account */
   /* todo set interlace pass in struct according to pass */
   switch (info->interlace_method)
   {
   case 0:
      scanline->color_samples = info->width;
      break;
   case 1:
      return ape_error;
   }

   /* alloc memory for last scanline, if needed */
   if (scanline->last_scanline == NULL ||
      (unsigned int)(scanline->lsl_size) < info->width * info->bytes_per_color)
   {
      scanline->lsl_size = info->width * info->bytes_per_color;

      new_data = (char*)realloc(scanline->last_scanline, scanline->lsl_size);
      if (new_data == NULL)
         return ape_memory;

      scanline->last_scanline = new_data;

      /* first scanline is all zeros */
      memset(scanline->last_scanline, 0, scanline->lsl_size);

      if (scanline->last_scanline == NULL)
         return ape_memory;
   }

   /* remember last scanline, but only if we begin a new line */
   if (scanline->sl_avail == 0)
   {
      if (scanline->scanline == NULL)
      {
         /* first scanline is all zeros */
         memset(scanline->last_scanline, 0, scanline->lsl_size);
      }
      else
      {
         memcpy(scanline->last_scanline, scanline->scanline,
            scanline->lsl_size);
      }

      scanline->sl_avail = scanline->sl_size;
   }

   /* alloc memory for scanline, if needed */
   if (scanline->scanline == NULL ||
      (unsigned int)(scanline->sl_size) < info->width * info->bytes_per_color)
   {
      scanline->sl_size = info->width * info->bytes_per_color;

      new_data = (char*)realloc(scanline->scanline, scanline->sl_size);
      if (new_data == NULL)
         return ape_memory;

      scanline->scanline = new_data;

      scanline->sl_avail = scanline->sl_size;
   }

   /* loop until the scanline is complete */
   do
   {
      /* step 1: check if we still have raw bytes */

      if (scanline->raw_count == 0)
      {
         if (scanline->stream_end)
            return ape_scanline_eos;

         /* no raw bytes available */
         if (scanline->zstream->next_in == NULL)
         {
            scanline->zstream->next_in = chunk->data;
            scanline->zstream->avail_in = chunk->length;
         }
         scanline->zstream->next_out = scanline->raw_buffer;
         scanline->zstream->avail_out = altpng_scanline_rawbufsize;

         err = inflate(scanline->zstream, Z_SYNC_FLUSH);
         if (err != Z_STREAM_END && err != Z_OK)
            return ape_zlib;

         if (err == Z_STREAM_END)
            scanline->stream_end = 1;

         /* check if we got output */
         if (scanline->zstream->avail_out ==
            (unsigned)altpng_scanline_rawbufsize)
         {
            scanline->zstream->next_in = NULL;

            /* need another chunk */
            return ape_scanline_nextchunk;
         }

         /* scanline->raw_count = scanline->zstream->next_out - scanline->raw_buffer; */
         scanline->raw_count = altpng_scanline_rawbufsize -
            scanline->zstream->avail_out;
      }

      /* step 2: now that we have data, we can try to decode some bytes to a
         scanline */

      source = scanline->raw_buffer;
      target = scanline->scanline + scanline->sl_size - scanline->sl_avail;

      /* when at start of line, copy filter type byte */
      if (scanline->line_start)
      {
         scanline->filter_type = *source;
         source++;
         scanline->raw_count--;

         scanline->line_start = 0;
      }

      /* calculate size of bytes to move */
      size = scanline->raw_count > scanline->sl_avail ?
         scanline->sl_avail : scanline->raw_count;

      memmove(target, source, size);

      /* count */
      scanline->sl_avail -= size;
      scanline->raw_count -= size;

      /* move remaining raw bytes to front */
      if (scanline->raw_count > 0)
      {
         memmove(scanline->raw_buffer, source + size, scanline->raw_count);
      }

      /* ask for more input data when raw buffer gets empty */
      if (!scanline->stream_end && scanline->raw_count == 0 &&
         scanline->zstream->avail_in == 0)
      {
         scanline->zstream->next_in = NULL;
         return ape_scanline_nextchunk;
      }

   } while (scanline->sl_avail > 0);

   scanline->line_start = 1;

   /* as a last step, decode filter */

   altpng_scanline_filter_decode(scanline, info);

   return ape_ok;
}

/*! \brief helper function for paeth predictor filter */
int _altpng_paeth_predictor(int a, int b, int c)
{
   /* a = left, b = above, c = upper left */
   int p, pa, pb, pc;

   p = a + b - c;    /* initial estimate */
   pa = abs(p - a);  /* distances to a, b, c */
   pb = abs(p - b);
   pc = abs(p - c);

   pa = abs(b - c);
   pb = abs(a - c);
   pc = abs(a + b - c - c);

   /* return nearest of a,b,c, breaking ties in order a,b,c. */
   return pa <= pb && pa <= pc ? a : (pb <= pc ? b : c);
}

/*! note that the filter type of the scanline is then set to 0, which means the scanline
    is not filtered; the last_scanline parameter must be a scanline which is already of
    filter type 0; if NULL is passed for last_scanline, the function assumes that
    this is the first scanline to apply filtering */
int altpng_scanline_filter_decode(altpng_scanline* scanline,
   altpng_image_info* info)
{
   int bpp, maxpix, i, j;

   if (scanline == NULL || info == NULL)
      return ape_pointer;

   if (info->filter_method != 0)
      return ape_invalid_png;

   if (scanline->filter_type == 0)
      return ape_ok;

   if (scanline->filter_type > 4)
      return ape_invalid_png;

   bpp = info->bytes_per_color;
   maxpix = scanline->color_samples * bpp;

   for (i = 0; i < bpp; i++)
   {
      for (j = i; j < maxpix; j += bpp)
      {
         unsigned char left = j < bpp ? 0 : scanline->scanline[j - bpp];
         unsigned char prior = scanline->last_scanline == NULL ? 0 :
            scanline->last_scanline[j];
         int val = scanline->scanline[j];

         switch (scanline->filter_type)
         {
         case 1: /* sub */
            val += left;
            break;

         case 2: /* up */
            val += prior;
            break;

         case 3: /* average */
            val += ((left + prior) >> 1);
            break;

         case 4: /* paeth predictor */
         {
            unsigned char  upperleft =
               (scanline->last_scanline == NULL || j < bpp) ? 0 :
               scanline->last_scanline[j - bpp];

            int paeth = _altpng_paeth_predictor(left, prior, upperleft);
            val += paeth;
         }
         break;
         }

         scanline->scanline[j] = val & 0xff;
      }
   }

   scanline->filter_type = 0;

   return ape_ok;
}
