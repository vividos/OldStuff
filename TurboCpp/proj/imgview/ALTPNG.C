/*
   altpng - a versatile png reading and writing library
   Copyright (c) 2003,2004,2021 Michael Fink

*/
/*! \file altpng.c

   \brief altpng common source file

*/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <zlib.h>
#include "altpng.h"

/*! Returns the version string in the format "altpng x.x.x".
    \return version string.
*/
const char* altpng_version_get()
{
   return "altpng 0.4.0";
}

/*! \brief PNG file signature bytes. */
unsigned char altpng_png_signature[8] =
{
   137, 80, 78, 71, 13, 10, 26, 10
};

/*! Checks PNG signature and returns if it is intact.

    \param buffer buffer with at least 8 bytes from the start of the file
    \return ape_ok if signature is ok, ape_invalid_png else.
*/
int altpng_check_signature(unsigned char* buffer)
{
   return memcmp(buffer, altpng_png_signature, 8) != 0 ?
      ape_invalid_png : ape_ok;
}


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


/*! \brief Table of CRCs of all 8-bit messages. */
static unsigned long altpng_crc32_table[256];

/*! \brief Flag: has the table been computed? Initially false. */
int altpng_crc32_table_computed = 0;

/*! Make the table for a fast CRC. */
void altpng_crc32_init()
{
   unsigned long c;
   int n, k;

   for (n = 0; n < 256; n++)
   {
      c = (unsigned long)n;
      for (k = 0; k < 8; k++)
      {
         if (c & 1)
            c = 0xedb88320L ^ (c >> 1);
         else
            c = c >> 1;
      }
      altpng_crc32_table[n] = c;
   }

   altpng_crc32_table_computed = 1;
}

/*! Update a running CRC with the bytes buf[0..len-1]--the CRC
    should be initialized to all 1's, and the transmitted value
    is the 1's complement of the final running CRC (see the
    crc() routine below)).

    \param crc running crc value calculated in a previous run of
               altpng_crc32_crc or altpng_crc32_update
    \param buf buffer with more bytes to calculate CRC from
    \param len number of bytes to use for CRC calculation, in bytes
    \return calculated CRC value
*/
unsigned long altpng_crc32_update(unsigned long crc, unsigned char* buf,
   int len)
{
   unsigned long c = crc;
   int n;

   if (!altpng_crc32_table_computed)
      altpng_crc32_init();
   for (n = 0; n < len; n++) {
      c = altpng_crc32_table[(c ^ buf[n]) & 0xff] ^ (c >> 8);
   }
   return c;
}

/*! Return the CRC of the bytes buf[0..len-1].

    \param buf buffer with bytes to calculate CRC from
    \param len number of bytes to use for CRC calculation, in bytes
    \return calculated CRC value
*/
unsigned long altpng_crc32_crc(unsigned char* buf, int len)
{
   return altpng_crc32_update(0xffffffffL, buf, len) ^ 0xffffffffL;
}


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


void altpng_image_info_init(altpng_image_info* image_info)
{
   memset(image_info, 0, sizeof(*image_info));
}

void altpng_image_info_finish(altpng_image_info* image_info)
{
   if (image_info->palette != NULL)
   {
      free(image_info->palette);
      image_info->palette = NULL;
   }
}

/*! Parses image info from given chunk and verifies image type and bit depth.
    \param image_info image info struct
    \param chunk IHDR chunk to parse infos
    \return ape_pointer if a passed pointer was invalid, ape_error if the
            chunk type was invalid or ape_invalid_png if the PNG file has
            invalid image properties or types.
*/
int altpng_image_info_parse(altpng_image_info* image_info,
   altpng_chunk* chunk)
{
   altpng_uint32* num;
   int bits_per_value = 0;
   int values_per_color = 0;
   int bit_depth = 0;

   if (image_info == NULL || chunk == NULL || chunk->data == NULL)
      return ape_pointer;

   /* check chunk type */
   if (memcmp(chunk->name, "IHDR", 4) != 0 || chunk->length != 13)
      return ape_error;

   /* retrieve the image info values from the header */

   num = (altpng_uint32*)(chunk->data) + 0;
   image_info->width = altpng_endian_convert32(*num);

   num = (altpng_uint32*)(chunk->data) + 1;
   image_info->height = altpng_endian_convert32(*num);

   image_info->bit_depth = bit_depth = ((altpng_uint8*)chunk->data)[8];
   image_info->color_type = ((altpng_uint8*)chunk->data)[9];
   image_info->compression_method = ((altpng_uint8*)chunk->data)[10];
   image_info->filter_method = ((altpng_uint8*)chunk->data)[11];
   image_info->interlace_method = ((altpng_uint8*)chunk->data)[12];

   /* calculate number of bytes forming a color sample */

   bits_per_value = image_info->bit_depth;

   switch (image_info->color_type)
   {
   case 0: /* grayscale sample */
      values_per_color = 1;
      break;
   case 2: /* RGB triple */
      values_per_color = 3;
      break;
   case 3: /* palette indexed */
      values_per_color = 1;
      bits_per_value = 8;
      break;
   case 4: /* grayscale with alpha sample */
      values_per_color = 2;
      break;
   case 6: /* RGB triple with alpha */
      values_per_color = 4;
      break;
   default:
      return ape_invalid_png;
   }

   image_info->bytes_per_color = (bits_per_value * values_per_color) >> 3;

   /* check for invalid values */

   switch (image_info->color_type)
   {
   case 0: // allowed: 1,2,4,8,16
      if (bit_depth != 1 && bit_depth != 2 &&
         bit_depth != 4 && bit_depth != 8 &&
         bit_depth != 16)
         return ape_invalid_png;
      break;

   case 3: // allowed: 1,2,4,8,16
      if (bit_depth != 1 && bit_depth != 2 &&
         bit_depth != 4 && bit_depth != 8)
         return ape_invalid_png;
      break;

   case 2: // allowed: 8,16
   case 4:
   case 6:
      if (bit_depth != 8 && bit_depth != 16)
         return ape_invalid_png;
   }

   image_info->palette_indices = 0;
   image_info->palette = NULL;

   return ape_ok;
}

/*! \todo check for valid palette length, calculate palette index count */
int altpng_image_info_get_palette(altpng_image_info* image_info,
   altpng_chunk* chunk)
{
   image_info->palette = chunk->data;
   chunk->data = NULL;

   image_info->palette_indices = chunk->length / 3;

   return ape_ok;
}


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


/*! Dummy chunk processing callback */
#pragma argsused
int altpng_callback_process_chunk(altpng_decoder* decoder,
   altpng_chunk* chunk)
{
   return 0;
}

/*! Dummy scanline processing callback */
#pragma argsused
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
