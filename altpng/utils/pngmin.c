/*
   altpng - a versatile png reading and writing library
   Copyright (c) 2003,2004,2021 Michael Fink

*/
/*! \file pngmin.c

   minimizes png files, using the best compression and scanline filters on
   the image, as well as leaving out unneeded chunks
   TODO: implement the best compression / scanline filters selection

*/

#include "altpng.h"
#include <string.h>


typedef struct pngmin_options
{
   const char* infile;
   const char* outfile;
} pngmin_options;


int pngmin_process(pngmin_options* opt)
{
   FILE* fin, * fout;

   fin = fopen(opt->infile, "rb");
   fout = fopen(opt->outfile, "wb");

   if (fin == NULL || fout == NULL)
      return 1;

   do
   {
      altpng_stream instr, outstr;
      altpng_chunk chunk;
      char signature[8];
      int exit = 0;

      /* init streams and chunks */
      altpng_stream_init(&instr, 1, fin);
      altpng_stream_init(&outstr, 0, fout);
      altpng_chunk_init(&chunk);

      altpng_stream_read(&instr, signature, 8);

      if (ape_ok != altpng_check_signature(signature))
         break;

      altpng_stream_write(&outstr, signature, 8);

      /* read first chunk */
      while (!exit)
      {
         altpng_chunk_load(&chunk, &instr);

         if (strcmp("IEND", chunk.name) == 0)
            exit = 1;

         if (strcmp("IHDR", chunk.name) == 0 ||
            strcmp("PLTE", chunk.name) == 0 ||
            strcmp("IDAT", chunk.name) == 0 ||
            strcmp("IEND", chunk.name) == 0)
         {
            altpng_chunk_save(&chunk, &outstr);
         }
      }

   } while (0);

   fclose(fin);
   fclose(fout);

   return 0;
}


int main(int argc, char** argv)
{
   pngmin_options opt;

   if (argc <= 2)
   {
      printf("syntax:\n %s <infile> <outfile>", argv[0]);
      return 0;
   }

   opt.infile = argv[1];
   opt.outfile = argv[2];

   pngmin_process(&opt);

   return 0;
}
