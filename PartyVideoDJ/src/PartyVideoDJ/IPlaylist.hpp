//
// PartyVideoDJ - YouTube crossfading app
// Copyright (C) 2008-2017 Michael Fink
//
/// \file IPlaylist.hpp Interface to playlist
//
#pragma once

/// type for video id's
typedef int T_VideoId;

class IPlaylist : public boost::noncopyable
{
public:
   virtual ~IPlaylist() throw() {}

   /// returns current playlist entry
   virtual T_VideoId GetCurrentEntry() const throw() = 0;

   /// returns if next position (current+1) has a valid entry
   virtual bool IsNextAvail() const throw() = 0;

   /// returns next playlist entry
   virtual T_VideoId GetNextEntry() const throw() = 0;

   /// sets next video to current, increases next pos
   virtual void NextToCurrent() throw() = 0;

   /// increases next position
   virtual void IncNextPos() throw() = 0;
};
