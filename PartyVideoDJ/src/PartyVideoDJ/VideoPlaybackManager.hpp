//
// PartyVideoDJ - YouTube crossfading app
// Copyright (C) 2008-2017 Michael Fink
//
/// \file VideoPlaybackManager.hpp Manager for video playback
//
#pragma once

#include "VideoInfo.hpp"

class IPlaybackApp;
class IPlaylistProvider;

class VideoPlaybackManager
{
public:
   VideoPlaybackManager(IPlaybackApp& app, IPlaylistProvider& playlistProvider) throw();
   ~VideoPlaybackManager() throw() {}

   enum T_enPlaybackState
   {
      stateStopped = 0, ///< playback was stopped
      statePaused, ///< playback was paused
      statePlaying, ///< playing video, waiting for next crossfade point
      stateCrossfade, ///< crossfading between two videos
   };

   T_enPlaybackState GetPlaybackState() const throw() { return m_enPlaybackState; }

   /// returns crossfade time in ms
   DWORD GetCrossfadeTime() const throw() { return m_dwCrossfadeTime; }

   void Start();
   void Pause();
   void Crossfade(); //< forces crossfade now, then playing back next video
   void Stop();

   void Tick();

   bool IsChangedPlaylist() const throw() { return m_bPlaylistChanged; }

   bool TickCrossfaded() const throw() { return m_bCrossfaded; }

private:
   void SwitchVideos();

   void SetupPreloadVideo(int iId);

   void TickPlaying();
   void TickCrossfade();

private:
   T_enPlaybackState m_enPlaybackState;
   DWORD m_dwCrossfadeTime;
   DWORD m_dwCrossfadeStart;

   double m_dCurrentFadeoutValue;
   bool m_bCheckNextVideo; ///< when true, check if next video becomes available again
   unsigned int m_uiNextVideoId;

   IPlaybackApp& m_app;
   IPlaylistProvider& m_playlistProvider;

   bool m_bPlaylistChanged;

   bool m_bCrossfaded;
};
