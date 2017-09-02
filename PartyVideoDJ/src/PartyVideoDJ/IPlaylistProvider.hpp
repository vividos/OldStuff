//
// PartyVideoDJ - YouTube crossfading app
// Copyright (C) 2008-2017 Michael Fink
//
/// \file IPlaylistProvider.hpp Interface to playlist provider
//
#pragma once

#include "VideoInfo.hpp"

class IPlaylist;

/// type for video id's
typedef int T_VideoId;

class IPlaylistProvider
{
public:
   virtual ~IPlaylistProvider() throw()
   {
   }

   virtual bool IsInit() const = 0;

   virtual VideoInfo GetVideoInfo(T_VideoId id) = 0;

   virtual std::shared_ptr<IPlaylist> GetCurrentPlaylist() = 0;
};
