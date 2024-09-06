#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#define TESTPICS_FOLDER "...\\OldStuff\\TurboCpp\\proj\\imgview\\pics\\"
#define OUTPUT_FOLDER "...\\temp\\"

#include "image.h"
#include "imgload.h"
#include <vector>

/// writes TGA file header
inline void tga_writeheader(FILE* fd, unsigned short width, unsigned short height, unsigned char type = 2, unsigned char colmap = 0, bool bottomup = false)
{
#pragma pack(push,1)

  // tga header struct
  struct tgaheader
  {
    unsigned char idlength;     // length of id field after header
    unsigned char colormaptype; // 1 if a color map is present
    unsigned char tgatype;      // tga type

    // colormap not used
    unsigned short colormaporigin;
    unsigned short colormaplength;
    unsigned char colormapdepth;

    // x and y origin
    unsigned short xorigin, yorigin;
    // width and height
    unsigned short width, height;

    // bits per pixel, either 16, 24 or 32
    unsigned char bitsperpixel;
    unsigned char imagedescriptor;
  } tgaheader =
  {
     0,
     colmap,
     type,

     0,
     (unsigned short)(colmap == 1 ? 256 : 0),
     (unsigned char)(colmap == 1 ? 24 : 0),

     0, 0,
     width, height,

     (unsigned char)(colmap == 1 ? 8 : 24),
     (unsigned char)(bottomup ? 0x00 : 0x20)
  };
#pragma pack(pop)

  fwrite(&tgaheader, 1, 18, fd);
}

namespace imgview
{
   TEST_CLASS(imgview)
   {
   public:
      TEST_METHOD(TestLoadGif_ByImage)
      {
        image* img = image::load(TESTPICS_FOLDER "cube.gif");
        Assert::IsNotNull(img, L"loaded image must be non-null");

        // write to tga
        FILE* tga = fopen(OUTPUT_FOLDER "from_gif_image.tga", "wb");
        tga_writeheader(tga, img->get_xres(), img->get_yres(), 1, 1);

        // write palette
        for (size_t i = 0; i < 256 * 3; i += 3)
        {
          std::swap(img->get_palette()[i + 0], img->get_palette()[i + 2]);
        }
        fwrite(img->get_palette(), 1, 256 * 3, tga);
        fwrite(img->get_data(), 1, img->get_xres() * img->get_yres(), tga);
        fclose(tga);
      }

      TEST_METHOD(TestLoadGif_ByScanline)
      {
        const char* filename = TESTPICS_FOLDER "cube.gif";
        FILE* fd = fopen(filename, "rb");
        image_loader* loader = image_loader::get_image_loader(filename, fd);
        Assert::IsNotNull(loader, L"image loader must be non-null");

        word xres = 0, yres = 0;
        byte bits_per_pixel = 0;
        bool has_palette = false;
        byte palette[256][3] = {};

        bool ret = loader->read_metadata(xres, yres, bits_per_pixel, has_palette, palette);
        Assert::IsTrue(ret, L"reading metadata must succeed");

        std::vector<byte> data;
        data.resize(xres * yres);

        int y;
        do
        {
          y = loader->get_next_y();
          if (y < 0)
            break;

          byte* pixbuf = &data[xres * y];

          bool ret2 = loader->read_scanline(pixbuf, xres);

          Assert::IsTrue(ret2, L"reading scanline must succeed");

        } while (y >= 0);

        // write to tga
        FILE* tga = fopen(OUTPUT_FOLDER "from_gif_scanline.tga", "wb");
        tga_writeheader(tga, xres, yres, 1, 1);

        // write palette
        for (size_t i = 0; i < 256; i++)
        {
          std::swap(palette[i][0], palette[i][2]);
        }
        fwrite(palette, 1, 256 * 3, tga);

        fwrite(data.data(), 1, xres * yres, tga);
        fclose(tga);

        fclose(fd);
      }

      TEST_METHOD(TestLoadJpeg_ByImage)
      {
        image* img = image::load(TESTPICS_FOLDER "cube.jpg");
        Assert::IsNotNull(img, L"loaded image must be non-null");

        // write to tga
        FILE* tga = fopen(OUTPUT_FOLDER "from_jpeg_image.tga", "wb");
        tga_writeheader(tga, img->get_xres(), img->get_yres(), 2, 0);

        fwrite(img->get_data(), 1, img->get_xres() * img->get_yres() * 3, tga);
        fclose(tga);
      }

      TEST_METHOD(TestLoadJpeg_ByScanline)
      {
        const char* filename = TESTPICS_FOLDER "cube.jpg";
        FILE* fd = fopen(filename, "rb");
        image_loader* loader = image_loader::get_image_loader(filename, fd);
        Assert::IsNotNull(loader, L"image loader must be non-null");

        word xres = 0, yres = 0;
        byte bits_per_pixel = 0;
        bool has_palette = false;
        byte palette[256][3] = {};

        bool ret = loader->read_metadata(xres, yres, bits_per_pixel, has_palette, palette);
        Assert::IsTrue(ret, L"reading metadata must succeed");

        std::vector<byte> data;
        data.resize(xres * yres * 3);

        int y;
        do
        {
          y = loader->get_next_y();
          if (y < 0)
            break;

          byte* pixbuf = &data[xres * y * 3];

          bool ret2 = loader->read_scanline(pixbuf, xres * 3);

          Assert::IsTrue(ret2, L"reading scanline must succeed");

        } while (y >= 0);

        // write to tga
        FILE* tga = fopen(OUTPUT_FOLDER "from_jpeg_scanline.tga", "wb");
        tga_writeheader(tga, xres, yres, 2, 0);

        fwrite(data.data(), 1, xres * yres * 3, tga);
        fclose(tga);

        fclose(fd);
      }

      TEST_METHOD(TestLoadPng8bit_ByImage)
      {
        image* img = image::load(TESTPICS_FOLDER "cube.png");
        Assert::IsNotNull(img, L"loaded image must be non-null");

        // write to tga
        FILE* tga = fopen(OUTPUT_FOLDER "from_png_image.tga", "wb");
        tga_writeheader(tga, img->get_xres(), img->get_yres(), 1, 1);

        // write palette
        for (size_t i = 0; i < 256 * 3; i += 3)
        {
          std::swap(img->get_palette()[i + 0], img->get_palette()[i + 2]);
        }
        fwrite(img->get_palette(), 1, 256 * 3, tga);

        fwrite(img->get_data(), 1, img->get_xres() * img->get_yres(), tga);
        fclose(tga);
      }
   };
}
