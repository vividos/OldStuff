//
// PartyVideoDJ - YouTube crossfading app
// Copyright (C) 2008-2017 Michael Fink
//
/// \file TemporaryPlaylistProvider.cpp Temporary memory-only playlist provider
//
#include "stdafx.h"
#include "TemporaryPlaylistProvider.hpp"
#include <ulib/stream/TextFileStream.hpp>

CString GetTemporaryPlaylistFilename()
{
   CString cszFilename;
   GetModuleFileName(NULL, cszFilename.GetBuffer(MAX_PATH), MAX_PATH);
   cszFilename.ReleaseBuffer();

   int iPos = cszFilename.ReverseFind(_T('\\'));
   cszFilename = cszFilename.Left(iPos + 1);
   cszFilename += _T("playlist.txt");
   return cszFilename;
}

void TemporaryPlaylistProvider::Load()
{
   try
   {
      Stream::TextFileStream fs(GetTemporaryPlaylistFilename(),
         Stream::FileStream::modeOpen,
         Stream::FileStream::accessRead,
         Stream::FileStream::shareRead);

      CString cszLine, cszAddress, cszTitle;
      while (!fs.AtEndOfStream())
      {
         fs.ReadLine(cszLine);
         cszLine.Remove(_T('\ufeff'));
         if (cszLine.IsEmpty())
            continue;

         // split line
         int iPos = cszLine.Find(_T(' '));

         cszAddress = cszLine.Left(iPos);
         cszTitle = cszLine.Mid(iPos + 1);

         AddVideo(cszTitle, cszAddress);
      }
   }
   catch (...)
   {
   }
}

void TemporaryPlaylistProvider::Save()
{
   try
   {
      Stream::TextFileStream fs(GetTemporaryPlaylistFilename(),
         Stream::FileStream::modeCreate,
         Stream::FileStream::accessWrite,
         Stream::FileStream::shareRead);

      std::shared_ptr<IPlaylist> spIPlaylist = GetCurrentPlaylist();
      std::shared_ptr<Playlist> spPlaylist = std::dynamic_pointer_cast<Playlist>(spIPlaylist);
      Playlist& playlist = *spPlaylist;

      for (size_t i = 0, iMax = playlist.GetSize(); i < iMax; i++)
      {
         const PlaylistEntry& entry = playlist.GetEntry(i);
         const VideoInfo& info = GetVideoInfo(entry.m_iId);

         fs.Write(info.Address());
         fs.Write(_T(" "));
         fs.Write(info.Name());

         fs.WriteEndline();
      }
   }
   catch (...)
   {
   }
}
