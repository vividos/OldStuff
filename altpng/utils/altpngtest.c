/*
   altpng - a versatile png reading and writing library
   Copyright (c) 2003,2004,2021 Michael Fink

*/
/*! \file altpngtest.c

   tests all aspects of the altpng library

*/

#include "altpng.h"

#include <stdlib.h>
#include <string.h>


#define START_TEST \
   passed = 0; \
   do {

#define END_TEST \
   passed = 1; \
   } while(0); \
   if (!passed) return 1;

/*! Does all tests for altpng. A test is started with the START_TEST macro and
    ends with the END_TEST macro. If a test fails, leave it with the "break;"
    command.
*/
int altpngtest_do()
{
   int passed = 0;

   // basic stuff tests
   START_TEST
   {
      altpng_uint16 value1, value2;
      altpng_uint32 value3, value4;

      printf("testing basic stuff ... ");

      // checking endian conversion
      value1 = 0xfeed;
      value2 = 0x42ab;

#ifndef WORDS_BIGENDIAN
      if (altpng_endian_convert16(value1) != 0xedfe ||
          altpng_endian_convert16(value2) != 0xab42)
      {
         printf("failed at altpng_endian_convert16()!\n");
         break;
      }
#endif

      value3 = 0xfeedbeef;
      value4 = 0x1aadfa42;

#ifndef WORDS_BIGENDIAN
      if (altpng_endian_convert32(value3) != 0xefbeedfe ||
          altpng_endian_convert32(value4) != 0x42faad1a)
      {
         printf("failed at altpng_endian_convert32()!\n");
         break;
      }
#endif

      printf("ok!\n");
   }
   END_TEST

   // png signature check
   START_TEST
   {
      unsigned char test_sig[9] =
      {
         137, 80, 78, 71, 13, 10, 26, 10, 1
      };

      printf("testing png signature checking ... ");

      if (altpng_check_signature(&test_sig[0]) != ape_ok ||
          altpng_check_signature(&test_sig[1]) == ape_ok)
      {
         printf("failed at altpng_check_signature()!\n");
         break;
      }

      printf("ok!\n");
   }
   END_TEST

   // crc32 test
   START_TEST
   {
      unsigned long value1, value2;
      const char* test_string1 = "hello altpngtest!";
      const char* test_string2 = "here's another one for you!";

      printf("testing crc32 ... ");

      altpng_crc32_init();

      value1 = altpng_crc32_crc(NULL, 0);

      if (value1 != 0x00000000)
      {
         printf("failed at altpng_crc32_crc()!\n");
         break;
      }

      value1 = altpng_crc32_crc((unsigned char*)test_string1,
         strlen(test_string1)+1);

      if (value1 != 0xea91bf4b)
      {
         printf("failed at altpng_crc32_crc()!\n");
         break;
      }

      value2 = altpng_crc32_update(value1,
         (unsigned char*)test_string2, strlen(test_string2)+1);

      if (value2 != 0xda65d08e)
      {
         printf("failed at altpng_crc32_update()!\n");
         break;
      }

      printf("ok!\n");
   }
   END_TEST

   // chunk test
   START_TEST
   {

   }
   END_TEST

   START_TEST
   {

   }
   END_TEST

   // passed all tests
   return 0;
}


int main(int argc, char** argv)
{
   printf("altpngtest - altpng regression tests\n\n");

   printf("using %s\n\n", altpng_version_get());

   return altpngtest_do();
}
