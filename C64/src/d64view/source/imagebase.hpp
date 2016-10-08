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

   $Id: imagebase.hpp,v 1.8 2003/05/19 21:08:17 vividos Exp $

*/
/*! \file imagebase.hpp

   \brief disk image base class interface

   defines a generic base class for disk image loading; to create
   implementations to load files, just derive from "disk_image_base" and
   override load_image() to fill the data members of the interface class.

   note: all track and sector numbers start at offset 0, even for tracks (they
         normally begin at track 1).

*/

// import guard
#ifndef d64view_imagebase_hpp_
#define d64view_imagebase_hpp_


// classes

//! single disk block
struct disk_block
{
   std::vector<c64_byte> data; //!< block data bytes
};


//! directory entry
struct dir_entry
{
   std::string filename; //!< entry filename
   c64_byte type;        //!< file type byte
   unsigned int blocks;  //!< number of blocks occupied
   c64_byte track;       //!< starting track (zero-based)
   c64_byte sector;      //!< starting sector
};


//! abstract disk image interface
class disk_image_base
{
public:
   //! ctor
   disk_image_base(){}

   //! dtor
   virtual ~disk_image_base(){}

   //! loads disk image
   virtual bool load_image(const char* filename)=0;


   //! returns image filename
   inline const char* get_image_filename();

   //! maps from track/sector number to block number
   unsigned int map_to_blocks(unsigned int track,unsigned int sector);

   //! returns number of tracks of disk
   inline unsigned int get_num_tracks();

   //! returns number of sectors per track
   inline unsigned int get_sectors_per_track(unsigned int track);

   //! returns error code for given track and sector
   inline unsigned int get_error_code(unsigned int numblock);

   //! returns a disk block
   inline disk_block& get_disk_block(unsigned int numblock);

   //! returns if block is marked available
   inline bool get_block_avail(unsigned int numblock);

   //! returns number of directory entries
   inline unsigned int get_num_dir_entries();

   //! returns a directory entry
   inline const dir_entry& get_dir_entry(unsigned int index);

   //! returns number of free blocks
   inline unsigned int get_blocks_free();

   //! returns disk label (max. 16 chars)
   inline std::string& get_disk_label();

   //! returns disk id string (5 chars)
   inline std::string& get_disk_id();

protected:
   //! list with number of sectors per track
   std::vector<unsigned int> sectors_per_track;

   //! error codes for every sector
   std::vector<c64_byte> errorcodes;

   //! all disk blocks
   std::vector<disk_block> allblocks;

   //! block availability map
   std::vector<bool> bam;

   //! all directory entries
   std::vector<dir_entry> alldirentries;

   //! number of free blocks on disk
   unsigned int blocksfree;

   //! disk label
   std::string disk_label;

   //! disk id string
   std::string disk_id;

   //! file name of disk image
   std::string image_filename;
};


// inline methods

const char* disk_image_base::get_image_filename()
{
   return image_filename.c_str();
}

unsigned int disk_image_base::get_num_tracks()
{
   return sectors_per_track.size();
}

unsigned int disk_image_base::get_sectors_per_track(unsigned int track)
{
   return sectors_per_track[track];
}

unsigned int disk_image_base::get_error_code(unsigned int numblock)
{
   return errorcodes[numblock];
}

disk_block& disk_image_base::get_disk_block(unsigned int numblock)
{
   return allblocks[numblock];
}

bool disk_image_base::get_block_avail(unsigned int numblock)
{
   return bam[numblock];
}

unsigned int disk_image_base::get_num_dir_entries()
{
   return alldirentries.size();
}

const dir_entry& disk_image_base::get_dir_entry(unsigned int index)
{
   return alldirentries[index];
}

unsigned int disk_image_base::get_blocks_free()
{
   return blocksfree;
}

std::string& disk_image_base::get_disk_label()
{
   return disk_label;
}

std::string& disk_image_base::get_disk_id()
{
   return disk_id;
}


#endif // d64view_imagebase_hpp_
