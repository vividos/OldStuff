/*
   altpng - a versatile png reading and writing library
   Copyright (c) 2003,2004,2021 Michael Fink

*/
/*! \file image.c

   \brief Image info parsing implementation.

*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <string.h>
#include "image.h"

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
