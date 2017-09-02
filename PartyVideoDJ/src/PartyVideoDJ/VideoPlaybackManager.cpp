//
// PartyVideoDJ - YouTube crossfading app
// Copyright (C) 2008-2017 Michael Fink
//
/// \file VideoPlaybackManager.cpp Manager for video playback
//
#include "StdAfx.h"
#include "VideoPlaybackManager.hpp"
#include "IPlaybackApp.hpp"
#include "Playlist.hpp"
#include "IPlaylistProvider.hpp"
#include "YouTubeWebPageView.hpp"

VideoPlaybackManager::VideoPlaybackManager(IPlaybackApp& app, IPlaylistProvider& playlistProvider) throw()
   :m_enPlaybackState(stateStopped),
   m_dwCrossfadeTime(2000),
   m_dwCrossfadeStart(0),
   m_dCurrentFadeoutValue(0.0),
   m_bCheckNextVideo(false),
   m_uiNextVideoId(0),
   m_app(app),
   m_playlistProvider(playlistProvider),
   m_bPlaylistChanged(false),
   m_bCrossfaded(false)
{
}

void VideoPlaybackManager::Start()
{
   if (!m_playlistProvider.IsInit())
      return; // no video database set

   if (m_enPlaybackState == statePaused)
   {
      Pause(); // unpause
      return;
   }

   // find out first video to play
   std::shared_ptr<IPlaylist> spPlaylist = m_playlistProvider.GetCurrentPlaylist();
   T_VideoId id = spPlaylist->GetCurrentEntry();
   VideoInfo info = m_playlistProvider.GetVideoInfo(id);

   m_dCurrentFadeoutValue = info.Fadeout();

   // set up new playback window
   m_app.SetupPane(true, info); // playback
   m_app.GetPlaybackView().SetActionWhenReady(YouTubeWebPageView::actionPlayVideo);

   if (spPlaylist->IsNextAvail())
   {
      int iId = spPlaylist->GetNextEntry();
      SetupPreloadVideo(iId);
   }
   else
   {
      m_app.EmptyPane(false); // preload
      m_bCheckNextVideo = true;
      m_uiNextVideoId = 0;
   }

   m_app.OnUpdatePlaybackVideo(info.Name());

   // continue playing
   m_enPlaybackState = statePlaying;
   m_app.StartTicking();
}

void VideoPlaybackManager::Pause()
{
   if (m_enPlaybackState == statePaused)
   {
      m_enPlaybackState = statePlaying;
      m_app.GetPlaybackView().PlayVideo();
      m_app.StartTicking();
   }
   else
   {
      m_enPlaybackState = statePaused;
      m_app.GetPlaybackView().PauseVideo();
      m_app.StopTicking();
   }
}

void VideoPlaybackManager::Crossfade()
{
   m_enPlaybackState = stateCrossfade;
   m_dwCrossfadeStart = GetTickCount();

   // start to play preloaded video
   m_app.GetPreloadView().PlayVideo();
}

void VideoPlaybackManager::Stop()
{
   m_enPlaybackState = stateStopped;
   m_app.GetPlaybackView().PauseVideo();
   m_app.StopTicking();
}

void VideoPlaybackManager::Tick()
{
   m_bPlaylistChanged = false;
   m_bCrossfaded = false;

   if (!m_playlistProvider.IsInit())
      return; // no video database set

   if (m_enPlaybackState == statePlaying)
      TickPlaying();
   else
      if (m_enPlaybackState == stateCrossfade)
         TickCrossfade();

   // check if user paused or continued the playback
   YouTubeWebPageView& view = m_app.GetPlaybackView();

   if (m_enPlaybackState == statePlaying &&
      view.GetPlayerState() == YouTubeWebPageView::playerStatePaused)
   {
      m_enPlaybackState = statePaused;
   }

   if (m_enPlaybackState == statePaused &&
      view.GetPlayerState() == YouTubeWebPageView::playerStatePlaying)
   {
      m_enPlaybackState = statePlaying;
   }
}

void VideoPlaybackManager::TickPlaying()
{
   // check if next video is available
   std::shared_ptr<IPlaylist> spPlaylist = m_playlistProvider.GetCurrentPlaylist();
   if (!spPlaylist->IsNextAvail())
      return; // do nothing; no next song

   YouTubeWebPageView& view = m_app.GetPlaybackView();

   // check playback view if an error occured
   if (view.IsPlayerError())
   {
      // mark as error
      std::shared_ptr<Playlist> spSimplePlaylist = std::dynamic_pointer_cast<Playlist>(spPlaylist);
      Playlist& playlist = *spSimplePlaylist;

      PlaylistEntry& entry = playlist.GetEntry(playlist.GetCurrentPos());
      entry.m_bHasError = true;

      m_bPlaylistChanged = true;

      // try next video
      if (spPlaylist->IsNextAvail())
      {
         Crossfade();
         return;
      }
   }

   bool bUpdatePreloadVideo = false;
   if (m_bCheckNextVideo)
   {
      // we previously had no next video; set up preload page for new one
      m_bCheckNextVideo = false;
      bUpdatePreloadVideo = true;
   }

   int iNextId = spPlaylist->GetNextEntry();

   if (m_uiNextVideoId != 0 && m_uiNextVideoId != static_cast<unsigned int>(iNextId))
      bUpdatePreloadVideo = true; // update preload video when playlist has changed

   // we got signaled that preload video has to be updated, e.g. when video changed
   if (bUpdatePreloadVideo)
   {
      SetupPreloadVideo(iNextId);
   }

   // check preload view if an error occured
   YouTubeWebPageView& preloadView = m_app.GetPreloadView();
   if (preloadView.IsPlayerError())
   {
      // mark as error
      std::shared_ptr<Playlist> spSimplePlaylist = std::dynamic_pointer_cast<Playlist>(spPlaylist);
      Playlist& playlist = *spSimplePlaylist;

      PlaylistEntry& entry = playlist.GetEntry(playlist.GetNextPos());
      entry.m_bHasError = true;

      m_bPlaylistChanged = true;

      // try next video
      if (spPlaylist->IsNextAvail())
         spPlaylist->IncNextPos();
   }

   if (view.GetPlayerState() != YouTubeWebPageView::playerStatePlaying)
      return; // don't check before player is playing

   // check if we're at crossfade time point
   double dDuration = view.GetDuration();
   if (dDuration < 0.0)
      return; // duration not known yet

   double dCrossfadeStart = dDuration - m_dCurrentFadeoutValue - GetCrossfadeTime() / 1000.0 / 2.0;
   double dCurrentTime = view.GetCurrentTime();

   if (dCurrentTime >= dCrossfadeStart)
      Crossfade();
}

void VideoPlaybackManager::TickCrossfade()
{
   // set volumes based on elapsed crossfade time
   DWORD dwCurrent = GetTickCount();
   DWORD dwDelta = dwCurrent - m_dwCrossfadeStart;

   // if time is over
   if (dwDelta >= GetCrossfadeTime())
   {
      ATLTRACE(_T("crossfade done, swap videos\n"));
      SwitchVideos();
      m_bCrossfaded = true;
   }
   else
   {
      double dVolumeFadein = (100.0 * dwDelta) / GetCrossfadeTime();
      double dVolumeFadeout = 100.0 - dVolumeFadein;

      ATLTRACE(_T("crossfade at %u ms, fadein vol=%u, fadeout vol=%u\n"),
         dwDelta,
         static_cast<unsigned>(dVolumeFadein),
         static_cast<unsigned>(dVolumeFadeout));

      m_app.GetPlaybackView().SetVolume(static_cast<unsigned int>(dVolumeFadeout));
      m_app.GetPreloadView().SetVolume(static_cast<unsigned int>(dVolumeFadein));
   }
}

void VideoPlaybackManager::SwitchVideos()
{
   if (!m_playlistProvider.IsInit())
      return; // no video database set

   // find out next video to play
   std::shared_ptr<IPlaylist> spPlaylist = m_playlistProvider.GetCurrentPlaylist();

   CString cszVideoName;
   if (spPlaylist->IsNextAvail())
   {
      m_app.SwapPanes();

      spPlaylist->NextToCurrent();

      // set new fadeout value
      {
         T_VideoId id = spPlaylist->GetCurrentEntry();
         VideoInfo info = m_playlistProvider.GetVideoInfo(id);

         m_dCurrentFadeoutValue = info.Fadeout();
         cszVideoName = info.Name();
      }

      if (spPlaylist->IsNextAvail())
      {
         int iNextId = spPlaylist->GetNextEntry();
         SetupPreloadVideo(iNextId);

         m_uiNextVideoId = iNextId;
      }
      else
      {
         m_app.EmptyPane(false); // preload
         m_bCheckNextVideo = true;
         m_uiNextVideoId = 0;
      }

      // check if the (now) current pane shows an error
      if (m_app.GetPlaybackView().IsPlayerError())
      {
         // yes: modify views
         if (m_uiNextVideoId != 0)
         {
            VideoInfo info = m_playlistProvider.GetVideoInfo(m_uiNextVideoId);
            double dFadeinTime = info.Fadein();

            m_app.GetPreloadView().SetActionWhenReady(YouTubeWebPageView::actionPlayVideo, dFadeinTime);
         }

         m_app.GetPlaybackView().SetActionWhenReady(YouTubeWebPageView::actionDoNothing);

         // immediately start crossfading again
         Crossfade();

         m_app.OnUpdatePlaybackVideo(cszVideoName);
         return;
      }

      // continue with playing back state
      m_enPlaybackState = statePlaying;
   }
   else
   {
      // no next video: just stop playback
      m_enPlaybackState = stateStopped;
      m_app.EmptyPane(true); // playback
      m_bCheckNextVideo = true;
      m_uiNextVideoId = 0;
   }

   m_app.OnUpdatePlaybackVideo(cszVideoName);
}

void VideoPlaybackManager::SetupPreloadVideo(int iId)
{
   if (!m_playlistProvider.IsInit())
      return; // no video database set

   VideoInfo infoNext = m_playlistProvider.GetVideoInfo(iId);

   m_app.SetupPane(false, infoNext); // preload
   m_uiNextVideoId = infoNext.Id();

   // calculate start seek point
   double dFadeinTime = infoNext.Fadein();
   double dCrossfadeTimeHalf = GetCrossfadeTime() / 1000.0 / 2.0;
   if (dCrossfadeTimeHalf >= dFadeinTime)
      dFadeinTime = 0.0;
   else
      dFadeinTime -= dCrossfadeTimeHalf;

   m_app.GetPreloadView().SetActionWhenReady(YouTubeWebPageView::actionSeekAndPause, dFadeinTime);
}
