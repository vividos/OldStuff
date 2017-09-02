//
// PartyVideoDJ - YouTube crossfading app
// Copyright (C) 2008-2017 Michael Fink
//
/// \file MainFrame.hpp Application main frame
//
#pragma once

#include "IPlaybackApp.hpp"
#include "TemporaryPlaylistProvider.hpp"
#include "VideoPlaybackManager.hpp"
#include "ControlBarForm.hpp"
#include "YouTubeWebPageView.hpp"
#include "VideoSearchView.hpp"
#include "Screensaver.hpp"
#include "PlaylistCtrl.hpp"
#include "FullscreenHelper.hpp"
#include "Win7Taskbar.hpp"
#include "LogFile.hpp"

class MainFrame :
   public CFrameWindowImpl<MainFrame>,
   public CUpdateUI<MainFrame>,
   public CMessageFilter,
   public CIdleHandler,
   public IPlaybackApp,
   public FullscreenHelper,
   public boost::noncopyable
{
public:
   MainFrame();
   ~MainFrame();

   DECLARE_FRAME_WND_CLASS(NULL, IDR_MAINFRAME)

   virtual BOOL PreTranslateMessage(MSG* pMsg) override;
   virtual BOOL OnIdle() override;

   BEGIN_UPDATE_UI_MAP(MainFrame)
      UPDATE_ELEMENT(ID_VIEW_STATUS_BAR, UPDUI_MENUPOPUP)
   END_UPDATE_UI_MAP()

   BEGIN_MSG_MAP(MainFrame)
      MESSAGE_HANDLER(WM_CREATE, OnCreate)
      MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
      MESSAGE_HANDLER(WM_TIMER, OnTimer)
      MESSAGE_HANDLER(WM_ACTIVATE, OnActivate)
      COMMAND_ID_HANDLER(IDC_BUTTON_START_PAUSE, OnPlayStartPause)
      COMMAND_ID_HANDLER(IDC_BUTTON_NEXT, OnPlayNext)
      COMMAND_ID_HANDLER(IDC_BUTTON_SEARCH, OnViewSearch)
      COMMAND_ID_HANDLER(IDC_BUTTON_PLAYLIST, OnViewPlaylist)
      COMMAND_ID_HANDLER(ID_APP_EXIT, OnAppExit)
      COMMAND_ID_HANDLER(ID_APP_ABOUT, OnAppAbout)
      CHAIN_MSG_MAP(CUpdateUI<MainFrame>)
      CHAIN_MSG_MAP(CFrameWindowImpl<MainFrame>)
      CHAIN_MSG_MAP_MEMBER(m_lcPlaylist)
      REFLECT_NOTIFICATIONS()
   END_MSG_MAP()

   // Handler prototypes (uncomment arguments if needed):
   // LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
   // LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
   // LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

   LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
   LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
   LRESULT OnTimer(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
   LRESULT OnActivate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

   LRESULT OnPlayStartPause(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
   LRESULT OnPlayNext(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
   LRESULT OnViewSearch(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
   LRESULT OnViewPlaylist(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

   LRESULT OnAppExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
   LRESULT OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

   void SetFrameTitle(bool bIsPlaying, const CString& cszTitle);
   void UpdatePaneVisibility();
   void OnAddTitle(const CString& cszTitle, const CString& cszAddress);
   void OnSearchTitle(const CString& cszAddress);

private:
   /// add playback page
   void AddPlaybackPage();
   /// add search page
   void AddSearchPage();
   /// add playlist pane
   void AddPlaylistPane();

   /// switch to fullscreen
   void SwitchToFullscreen();

   /// switch to windowed mode
   void SwitchToWindowed();

   // virtual methods from IPlaybackApp

   virtual YouTubeWebPageView& GetPlaybackView() override;
   virtual YouTubeWebPageView& GetPreloadView() override;

   virtual void StartTicking() override;
   virtual void StopTicking() override;

   virtual void SetupPane(bool bPlaybackView, const VideoInfo& info) override;
   virtual void EmptyPane(bool bPlaybackView) override;
   virtual void SwapPanes() override;

   /// called when currently played back video has changed
   virtual void OnUpdatePlaybackVideo(const CString& cszNewVideoName) override;

private:
   TemporaryPlaylistProvider m_tempPlaylistProvider;

   /// video playback manager
   VideoPlaybackManager m_videoPlaybackManager;

   VideoPlaybackManager::T_enPlaybackState m_lastPlaybackState;


   HMENU m_hHiddenMenu;

   CCommandBarCtrl m_CmdBar;

   CHorSplitterWindow m_splitterAll;

   ControlBarForm m_controlBar;

   CSplitterWindow m_splitterLower;
   CPaneContainer m_panePlaylist;
   PlaylistCtrl m_lcPlaylist;

   // first view: playback
   CSplitterWindow m_splitterPlayback;
   YouTubeWebPageView m_viewFirst;
   YouTubeWebPageView m_viewSecond;
   bool m_fViewFirstIsPlayback;

   // second view: search
   VideoSearchView m_searchView;

   /// screensaver control
   Screensaver m_screensaverState;

   std::unique_ptr<Win7::Taskbar> m_scpTaskbar;
   std::unique_ptr<Win7::TaskbarProgressBar> m_scpTaskbarProgressBar;

   bool m_bViewSearch;
   bool m_bViewPlaylist;

   unsigned int m_uiIgnoreNextMouseMessageCount;

   LogFile m_logFile;
};
