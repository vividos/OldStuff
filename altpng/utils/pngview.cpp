/*
   altpng - a versatile png reading and writing library
   Copyright (c) 2003,2004,2021 Michael Fink

*/
/*! \file pngview.cpp

    SDL program to view png images

*/

#include "altpng.h"
#include <SDL/SDL.h>
#include <vector>


// global variables

unsigned int width, height;
std::vector<Uint8> allimage;
unsigned int curline;


// functions

enum altpng_convert_format
{
   apf_bgr8 = 0,
};

/*! converts scanline to given format */
int altpng_scanline_convert(altpng_scanline* scanline,
   altpng_image_info* info, altpng_convert_format fmt, char* outbuf)
{
   char* srcbuf = scanline->scanline;
   unsigned int src_bit = info->bit_depth;
   unsigned int dst_bit;
   unsigned int src_bytes, dst_bytes;

   src_bytes = info->bytes_per_color;

   switch (fmt)
   {
   case apf_bgr8:
      dst_bit = 8;
      dst_bytes = 3;
      break;
   }

   altpng_uint16 pixel[4] = { 0,0,0,0 };
   unsigned int max = scanline->color_samples;
   for (unsigned int i = 0; i < max; i++)
   {
      /* determine truecolor pixel values */
      switch (info->color_type)
      {
      case 2: /* RGB samples */
         pixel[0] = srcbuf[0];
         pixel[1] = srcbuf[1];
         pixel[2] = srcbuf[2];
         break;

      case 3: /* palette indexed */
      {
         altpng_uint8 palidx = srcbuf[0];
         pixel[0] = info->palette[palidx * 3 + 0];
         pixel[1] = info->palette[palidx * 3 + 1];
         pixel[2] = info->palette[palidx * 3 + 2];
      }
      break;

      case 6: /* RGBA samples */
         pixel[0] = srcbuf[0];
         pixel[1] = srcbuf[1];
         pixel[2] = srcbuf[2];
         pixel[3] = srcbuf[3];
         break;
      }

      /* convert to output format */
      switch (fmt)
      {
      case apf_bgr8:
         outbuf[0] = pixel[2];
         outbuf[1] = pixel[1];
         outbuf[2] = pixel[0];
         break;
      }

      srcbuf += src_bytes;
      outbuf += dst_bytes;
   }

   return 0;
}

int pngview_scanline_store(altpng_decoder_ptr decoder, altpng_scanline* scanline)
{
   // convert to 8 bit bgr and store
   altpng_scanline_convert(scanline, &decoder->image_info,
      apf_bgr8, reinterpret_cast<char*>(&allimage[curline * width * 3]));

   curline++;
   return 0;
}

bool load_image(const char* filename)
{
   // open the file
   FILE* fp = fopen(filename, "rb");
   if (fp == NULL)
      return false;

   altpng_decoder decoder;
   altpng_stream stream;

   int ret = 0;
   do
   {
      // init structs
      altpng_stream_init(&stream, 1, fp);
      altpng_decoder_init(&decoder, &stream, NULL);

      // decode header
      ret = altpng_decoder_header(&decoder);

      if (ret != 0)
         break;

      width = decoder.image_info.width;
      height = decoder.image_info.height;

      allimage.resize(width * height * 3);

      // set up scanline process callback
      curline = 0;
      decoder.process_scanline = pngview_scanline_store;

      // decode rest of the image
      ret = altpng_decoder_decode(&decoder);

      if (ret != 0)
         break;

   } while (false);

   // clean up
   altpng_decoder_finish(&decoder);
   altpng_stream_finish(&stream);

   // close the file
   fclose(fp);

   return ret == 0;
}

#ifdef __cplusplus
extern "C"
#endif
int main(int argc, char* argv[])
{
   if (argc < 2)
   {
      fprintf(stderr, "please specify image to show\n");
      return 1;
   }

   // init SDL
   if (SDL_Init(SDL_INIT_VIDEO) < 0)
   {
      fprintf(stderr, "error initializing video: %s\n", SDL_GetError());
      return 1;
   }

   // get info about video
   const SDL_VideoInfo* info = SDL_GetVideoInfo();
   if (!info)
   {
      fprintf(stderr, "error getting video info: %s\n", SDL_GetError());
      SDL_Quit();
      return 1;
   }

   load_image(argv[1]);

   // set video mode
   SDL_Surface* display = SDL_SetVideoMode(width, height, 24,
      SDL_SWSURFACE | SDL_HWSURFACE | SDL_HWACCEL | SDL_DOUBLEBUF);

   if (display == 0)
   {
      fprintf(stderr, "failed to set video mode: %s\n", SDL_GetError());
      SDL_Quit();
      return 1;
   }

   // set window caption
   SDL_WM_SetCaption("pngview", NULL);

   // display image
   SDL_Surface* image;
   {
      // load image into surface
      image = SDL_CreateRGBSurfaceFrom(&allimage[0],
         width, height, 24, width * 3, 0, 0, 0, 0);

      SDL_Rect dest;
      dest.x = 0;
      dest.y = 0;

      // copy image to display surface
      SDL_BlitSurface(image, NULL, display, &dest);

      // show image
      SDL_Flip(display);
   }

   bool can_exit = false;

   // main loop
   while (!can_exit)
   {
      // process events
      {
         SDL_Event event;

         // check for new event
         while (SDL_PollEvent(&event))
         {
            switch (event.type)
            {
            case SDL_QUIT:
               can_exit = true;
               break;
            }
         }
      }

      SDL_Delay(10);

      // draw screen
   }

   SDL_FreeSurface(image);

   // finish off SDL
   SDL_Quit();

   return 0;
}
