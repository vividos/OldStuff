/*
   altpng - a versatile png reading and writing library
   Copyright (c) 2003,2004,2021 Michael Fink

*/
/*! \file image.h

   \brief Image info function.

*/

#ifndef altpng_image_h_
#define altpng_image_h_

#include "util.h"
#include "chunk.h"


/* structs */

/*! \brief a struct that holds PNG image info */
/*! Methods that can be used with altpng_image_info:
    \sa altpng_image_info_parse()
*/
typedef struct altpng_image_info
{
   /*! \brief image width */
   altpng_uint32 width;

   /*! \brief image height */
   altpng_uint32 height;

   /*! \brief image bit depth */
   altpng_uint8 bit_depth;

   /*! \brief color type
       0: grayscale image
       2: RGB truecolor image
       3: palette indexed image
       4: grayscale with alpha image
       6: RGBA truecolor with alpha image
   */
   altpng_uint8 color_type;

   /*! \brief compression method
       0: deflate/inflate compression method
   */
   altpng_uint8 compression_method;

   /*! \brief filter method
       0: adaptive filtering with 5 different filters
   */
   altpng_uint8 filter_method;

   /*! \brief interlace method
       0: no interlace
       1: Adam7 interlace
   */
   altpng_uint8 interlace_method;

   /*! \brief indicates number of bytes forming a color sample */
   int bytes_per_color;

   /*! \brief number of palette indices */
   unsigned int palette_indices;

   /*! \brief palette */
   altpng_uint8* palette;

} altpng_image_info;


/* Functions */

void altpng_image_info_init(altpng_image_info* image_info);
void altpng_image_info_finish(altpng_image_info* image_info);


/*! \brief Parses image info from the IHDR chunk */
int altpng_image_info_parse(altpng_image_info* image_info, altpng_chunk* chunk);

/*! \brief Extracts palette from PLTE chunk */
int altpng_image_info_get_palette(altpng_image_info* image_info, altpng_chunk* chunk);


#endif
