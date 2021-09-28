// note, this file is only for documentation purposes; it only contains comments
/*!
\mainpage altpng Documentation

\section Introduction

   altpng is an alternative library to read and write png images. altpng
   allows for more flexibility by providing more ways to access png files,
   including chunk level support, scanline decoding and decoding whole
   files.

\section abc Usage

   aa

code 0:

\code
   altpng_uint16 value1;
   value1 = 0xfeed;
   altpng_endian_convert16(value1);

\endcode

code 1:

\code
   altpng_uint16 value1, value2;

   altpng_crc32_init();

   static unsigned char buffer[] = { 1, 2, 3 };

   value1 = altpng_crc32_crc(NULL, 0);
   value2 = altpng_crc32_update(value1, buffer, 3);

\endcode


*/
