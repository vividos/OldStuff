//
// PartyVideoDJ - YouTube crossfading app
// Copyright (C) 2008-2017 Michael Fink
//
/// \file IPlaybackApp.hpp Interface to playback application
//
#pragma once

class YouTubeWebPageView;
class VideoInfo;

class IPlaybackApp
{
public:
   virtual ~IPlaybackApp() throw() {}

   virtual YouTubeWebPageView& GetPlaybackView() = 0;
   virtual YouTubeWebPageView& GetPreloadView() = 0;

   virtual void StartTicking() = 0;
   virtual void StopTicking() = 0;

   virtual void SetupPane(bool bPlaybackView, const VideoInfo& info) = 0;
   virtual void EmptyPane(bool bPlaybackView) = 0;
   virtual void SwapPanes() = 0;

   /// called when currently played back video has changed
   virtual void OnUpdatePlaybackVideo(const CString& cszNewVideoName) = 0;
};
