//
// PartyVideoDJ - YouTube crossfading app
// Copyright (C) 2008-2017 Michael Fink
//
/// \file TemporaryPlaylistProvider.hpp Temporary memory-only playlist provider
//
#pragma once

#include "IPlaylistProvider.hpp"
#include "IPlaylist.hpp"
#include "Playlist.hpp"
#include <map>

class TemporaryPlaylistProvider : public IPlaylistProvider
{
public:
   TemporaryPlaylistProvider()
      :m_spPlaylist(new Playlist),
      m_iNextVideoId(1)
   {
      Load();
   }

   virtual ~TemporaryPlaylistProvider()
   {
      Save();
   }

   void AddVideo(const CString& cszTitle, const CString& cszAddress)
   {
      T_VideoId iVideoId = m_iNextVideoId++;

      VideoInfo videoInfo(cszTitle, cszAddress, 5, 5);
      videoInfo.Id(iVideoId); // set id

      m_mapVideoInfo.insert(std::pair<int, VideoInfo>(iVideoId, videoInfo));

      m_spPlaylist->Add(iVideoId, iVideoId, 0);
   }

   virtual bool IsInit() const
   {
      return m_spPlaylist->GetSize() > 0;
   }

   virtual VideoInfo GetVideoInfo(T_VideoId id)
   {
      std::map<int, VideoInfo>::const_iterator iter = m_mapVideoInfo.find(id);
      if (iter == m_mapVideoInfo.end())
         throw - 1;
      else
         return iter->second;
   }

   virtual std::shared_ptr<IPlaylist> GetCurrentPlaylist()
   {
      return m_spPlaylist;
   }

   void Load();
   void Save();


private:
   std::shared_ptr<Playlist> m_spPlaylist;

   std::map<int, VideoInfo> m_mapVideoInfo;
   T_VideoId m_iNextVideoId;
};
