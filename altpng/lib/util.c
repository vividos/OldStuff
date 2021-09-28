/*
   altpng - a versatile png reading and writing library
   Copyright (c) 2003,2004,2021 Michael Fink

*/
/*! \file util.c

   \brief Utility functions implementation.

*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>
#include "util.h"


/* Functions */

/*! Returns the version string in the format "altpng x.x.x".
    \return version string.
*/
const char* altpng_version_get()
{
   return PACKAGE" "VERSION;
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
