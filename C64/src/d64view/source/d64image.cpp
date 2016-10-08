/*
   d64view - a d64 disk image viewer
   Copyright (c) 2002,2003 Michael Fink

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

   $Id: d64image.cpp,v 1.7 2003/05/19 21:08:15 vividos Exp $

*/
/*! \file d64image.cpp

   \brief d64 disk image functions

   handles reading and processing d64 disk images

*/

// needed includes
#include "common.hpp"
#include "d64image.hpp"


// d64_image methods

bool d64_image::load_image(const char* filename)
{
   image_filename.assign(filename);

   // set up sectors_per_track list
   sectors_per_track.clear();
   {
      unsigned int ntrack;
      for(ntrack= 0; ntrack<=16; ntrack++) sectors_per_track.push_back(21);
      for(ntrack=17; ntrack<=23; ntrack++) sectors_per_track.push_back(19);
      for(ntrack=24; ntrack<=29; ntrack++) sectors_per_track.push_back(18);
      for(ntrack=30; ntrack<=34; ntrack++) sectors_per_track.push_back(17);
      // tracks 36..40 also exist for extended disk images, but we add them
      // only when needed
   }

   // open file
   FILE* fd = fopen(filename,"rb");
   if (fd==NULL)
      return false; // file not open

   // get file length
   fseek(fd,0L,SEEK_END);
   long fsize = ftell(fd);
   fseek(fd,0L,SEEK_SET);

   bool errcodes = false;
   unsigned int numblocks = 683;
   unsigned int blocksize = 256;
   unsigned int bam_track = 17; // zero-based track count
   unsigned int bam_sector = 0;

   // check which d64 type we have
   switch(fsize)
   {
   case 174848: // 35 track disk
      numblocks = 683;
      errcodes = false;
      break;

   case 175531: // 174848+683, 35 track disk with error codes
      numblocks = 683;
      errcodes = true;
      break;

   case 196608: // 40 track disk
      numblocks = 768;
      errcodes = false;

      // add tracks 36-40
      {
         for(unsigned int ntrack=35; ntrack<=39; ntrack++)
            sectors_per_track.push_back(17);
      }
      break;

   case 197376: // 196608+768, 40 track disk with error codes
      numblocks = 768;
      errcodes = true;

      // add tracks 36-40
      {
         for(unsigned int ntrack=35; ntrack<=39; ntrack++)
            sectors_per_track.push_back(17);
      }
      break;

   default:
      // unknown type
      return false;
   }

   // load d64 image
   allblocks.clear();

   // load all blocks
   {
      unsigned int track,sector,sector_max;
      track = sector = 0;
      sector_max = get_sectors_per_track(track);

      for(unsigned int n=0; n<numblocks; n++)
      {
         // read in block
         disk_block block;

         block.data.resize(blocksize);
         fread(&block.data[0],blocksize,1,fd);

         allblocks.push_back(block);

         // count up sectors/tracks
         ++sector;

         if (sector>=sector_max)
         {
            track++;
            sector = 0;
            sector_max = get_sectors_per_track(track);
         }
      }
   }

   // read in error codes
   {
      errorcodes.clear();
      errorcodes.resize(numblocks,0x01);

      if (errcodes)
         fread(&errorcodes[0],numblocks,1,fd);
   }

   // extract bam entries / disk label
   {
      bam.clear();
      bam.resize(numblocks,false);
      blocksfree = 0;

      unsigned int bam_blockno = map_to_blocks(bam_track,bam_sector);
      const disk_block& bam_block = get_disk_block(bam_blockno);

      // get disk label
      disk_label.assign(reinterpret_cast<const char*>(&bam_block.data[0x90]),16);

      std::string::size_type pos = disk_label.find((char)0xa0,0);
      if (pos!=std::string::npos)
         disk_label.erase(pos);

      // get disk id
      disk_id.assign(reinterpret_cast<const char*>(&bam_block.data[0xa2]),16);
      if (disk_id[2]==(char)0xa0) disk_id[2] = ' ';

      // extract bam bits
      unsigned int max = get_num_tracks();
      for(unsigned int track=0; track<max; track++)
      {
         const c64_byte* bamptr = &bam_block.data[0x04 + track*4];

         // first byte is number of free blocks in this track
         blocksfree += *bamptr++;

         c64_byte curbyte = *bamptr;
         unsigned int bits = 8;

         unsigned int sector_max = get_sectors_per_track(track);
         for(unsigned int sector=0; sector<sector_max|| sector<35; sector++)
         {
            // get next byte if needed
            if (bits==0)
            {
               bamptr++;
               curbyte = *bamptr;
               bits = 8;
            }

            // get next bit
            bool free = (curbyte&1)==1;
            curbyte >>= 1;
            bits--;

            unsigned int blockno = map_to_blocks(track,sector);

            // check if we had enough 'bam bits'
            if (blockno>=numblocks)
               break;

            bam[blockno] = free;
         }
      }
   }

   // extract directory entries
   {
      unsigned int dir_track = bam_track+1;
      unsigned int dir_sector = 1;

      do
      {
         // get next block
         unsigned int dir_blockno = map_to_blocks(dir_track-1,dir_sector);
         disk_block dir_block = get_disk_block(dir_blockno);

         // extract dir infos
         for(unsigned int n=0; n<8; n++)
         {
            c64_byte* dirptr = &dir_block.data[n*32];

            if (dirptr[0x02]==0)
               continue; // scratched, skip this

            // create directory entry
            dir_entry entry;

            entry.type = dirptr[0x02];
            entry.filename.assign(reinterpret_cast<char*>(dirptr+0x05),16);

            std::string::size_type pos = entry.filename.find((char)0xa0,0);
            if (pos!=std::string::npos)
               entry.filename.erase(pos);

            entry.blocks = dirptr[0x1E] | unsigned(dirptr[0x1F])<<8;

            // TODO: extract more infos

            alldirentries.push_back(entry);
         }

         // get next block track/sector
         dir_track = dir_block.data[0];
         dir_sector = dir_block.data[1];

      } while(dir_track != 0);
   }

   fclose(fd);

   return true;
}
