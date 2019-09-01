//
// PartyVideoDJ - YouTube crossfading app
// Copyright (C) 2008-2017 Michael Fink
//
/// \file MainFrame.cpp Application main frame
//
#include "stdafx.h"
#include "resource.h"
#include "AboutDlg.hpp"
#include "MainFrame.hpp"
#include <functional>
#include "IconList.hpp"

// constants
#define IDT_PLAYBACK_TICK 100
#define IDT_MOUSE_MESSAGE_CHECK 101

const UINT c_uiTickTime = 200;

const unsigned int c_uiAutoFullscreenInSeconds = 30;

MainFrame::MainFrame()
   :m_videoPlaybackManager(*this, m_tempPlaylistProvider),
   m_fViewFirstIsPlayback(true),
   m_bViewSearch(false),
   m_bViewPlaylist(true),
   m_lcPlaylist(m_tempPlaylistProvider),
   m_hHiddenMenu(NULL),
   m_uiIgnoreNextMouseMessageCount(0)
{
#ifdef _DEBUG
   //m_tempPlaylistProvider.AddVideo(_T("Razorlight - Wire To Wire"), _T("m8K28GXcpP8"));
   //m_tempPlaylistProvider.AddVideo(_T("Razorlight - Wire To Wire"), _T("Rov1iz1xmGs"));
   //m_tempPlaylistProvider.AddVideo(_T(""), _T("xxx"));
   //m_tempPlaylistProvider.AddVideo(_T("Calvin Harris - Ready For The Weekend"), _T("PY9g36_Ez4Y"));
   //m_tempPlaylistProvider.AddVideo(_T("daft punk is playing at my house - lcd soundsystem"), _T("DbaOFkC8tQE"));
   //m_tempPlaylistProvider.AddVideo(_T("Stereo Total - Liebe Zu Dritt"), _T("Q3O08RieWwQ"));
   //m_tempPlaylistProvider.AddVideo(_T("My Brightest Diamond - Dragonfly"), _T("MwA96dZm6Mw"));
   //m_tempPlaylistProvider.AddVideo(_T("Clara Luzia - Queen of the Wolves"), _T("s309mucilT0"));
   //m_tempPlaylistProvider.AddVideo(_T("Martin Solveig & Dragonette - Hello"), _T("kK42LZqO0wA"));
   //m_tempPlaylistProvider.AddVideo(_T("Axel Fischer - Traum von Amsterdam"), _T("3aZcZ2Um2Yg"));
   //m_tempPlaylistProvider.AddVideo(_T("Calvin Harris - Ready For The Weekend"), _T("PY9g36_Ez4Y"));
#endif

   m_searchView.SetAddCallback(std::bind(&MainFrame::OnAddTitle, this, std::placeholders::_1, std::placeholders::_2));
   m_controlBar.SetSearchCallback(std::bind(&MainFrame::OnSearchTitle, this, std::placeholders::_1));
}

MainFrame::~MainFrame()
{
   m_controlBar.SetSearchCallback();
   m_searchView.SetAddCallback();
}

bool g_bInitSplitterAll = false;

BOOL MainFrame::PreTranslateMessage(MSG* pMsg)
{
   if (g_bInitSplitterAll)
   {
      m_splitterAll.SetSplitterPos(80);
      g_bInitSplitterAll = false;
   }

   if (pMsg->message == WM_KEYUP && pMsg->wParam == VK_F11)
   {
      if (!IsFullscreen(m_hWnd))
      {
         m_uiIgnoreNextMouseMessageCount = 1;

         SwitchToFullscreen();

         KillTimer(IDT_MOUSE_MESSAGE_CHECK);
      }
      else
         SwitchToWindowed();

      ToggleFullscreen(m_hWnd);
   }

   // check for mouse message
   if (pMsg->message >= WM_MOUSEFIRST && pMsg->message <= WM_MOUSELAST)
   {
      //ATLTRACE(_T("mouse message; wm=%02x\n"), pMsg->message);

      if (m_uiIgnoreNextMouseMessageCount > 0)
         m_uiIgnoreNextMouseMessageCount--;
      else
      {
         //ATLTRACE(_T("mouse message; restart timer\n"));

         // restart timer
         KillTimer(IDT_MOUSE_MESSAGE_CHECK);
         SetTimer(IDT_MOUSE_MESSAGE_CHECK, c_uiAutoFullscreenInSeconds * 1000);

         if (IsFullscreen(m_hWnd))
         {
            ATLTRACE(_T("go to windowed\n"));

            SwitchToWindowed();
            ToggleFullscreen(m_hWnd);
         }
      }
   }

   if (CFrameWindowImpl<MainFrame>::PreTranslateMessage(pMsg))
      return TRUE;

   BOOL bRet = m_controlBar.PreTranslateMessage(pMsg);
   if (bRet)
      return TRUE;

   return GetPlaybackView().PreTranslateMessage(pMsg);
}

BOOL MainFrame::OnIdle()
{
   return FALSE;
}

LRESULT MainFrame::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   m_hWndClient = m_splitterAll.Create(m_hWnd, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

   // set fixed splitter position
   m_splitterAll.SetSplitterExtendedStyle(0);
   g_bInitSplitterAll = true;
   m_splitterAll.m_cxySplitBar = 0;

   m_controlBar.Create(m_splitterAll);
   m_splitterLower.Create(m_splitterAll, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

   m_splitterAll.SetSplitterPanes(m_controlBar, m_splitterLower);

   AddPlaybackPage();
   AddSearchPage();
   AddPlaylistPane();

   m_splitterLower.SetSplitterPanes(m_splitterPlayback, m_panePlaylist);
   UpdateLayout();
   m_splitterLower.SetSplitterPosPct(65);

   // set check buttons
   m_controlBar.SetSearchButton(m_bViewSearch);
   m_controlBar.SetPlaylistButton(m_bViewPlaylist);


   // register object for message filtering and idle updates
   CMessageLoop* pLoop = _Module.GetMessageLoop();
   ATLASSERT(pLoop != NULL);
   pLoop->AddMessageFilter(this);
   pLoop->AddIdleHandler(this);

   // create taskbar object
   m_scpTaskbar.reset(new Win32::Taskbar(m_hWnd));

   return 0;
}

LRESULT MainFrame::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
   KillTimer(IDT_PLAYBACK_TICK);
   KillTimer(IDT_MOUSE_MESSAGE_CHECK);

   // unregister message filtering and idle updates
   CMessageLoop* pLoop = _Module.GetMessageLoop();
   ATLASSERT(pLoop != NULL);
   pLoop->RemoveMessageFilter(this);
   pLoop->RemoveIdleHandler(this);

   bHandled = FALSE;
   return 1;
}

LRESULT MainFrame::OnTimer(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   if (wParam == IDT_PLAYBACK_TICK)
   {
      m_videoPlaybackManager.Tick();

      if (m_videoPlaybackManager.IsChangedPlaylist())
         m_lcPlaylist.UpdatePlaylist();

      if (m_videoPlaybackManager.TickCrossfaded())
         m_uiIgnoreNextMouseMessageCount = 2;

      // check if playback status changed
      VideoPlaybackManager::T_enPlaybackState currentPlaybackState =
         m_videoPlaybackManager.GetPlaybackState();

      if ((currentPlaybackState == VideoPlaybackManager::statePlaying ||
         currentPlaybackState == VideoPlaybackManager::statePaused) &&
         m_lastPlaybackState != currentPlaybackState)
      {
         // changed state: update play/pause button
         bool bPlaying = currentPlaybackState != VideoPlaybackManager::statePaused;
         m_controlBar.SetStartPauseButton(!bPlaying);

         if (m_scpTaskbarProgressBar != NULL)
            m_scpTaskbarProgressBar->SetState(bPlaying ?
               Win32::TaskbarProgressBar::TBPF_NORMAL : Win32::TaskbarProgressBar::TBPF_PAUSED);
      }

      m_lastPlaybackState = m_videoPlaybackManager.GetPlaybackState();

      if (m_scpTaskbarProgressBar != NULL)
      {
         YouTubeWebPageView& view = GetPlaybackView();
         double dCurrent = view.GetCurrentTime();
         double dDuration = view.GetDuration();

         bool bPlaying = currentPlaybackState != VideoPlaybackManager::statePaused;

         if (bPlaying)
            m_scpTaskbarProgressBar->SetPos(unsigned(dCurrent), unsigned(dDuration));

         m_scpTaskbarProgressBar->SetState(bPlaying ?
            Win32::TaskbarProgressBar::TBPF_NORMAL : Win32::TaskbarProgressBar::TBPF_PAUSED);

         //ATLTRACE(_T("Win32 taskbar: pos=%u/%u playing=%u\n"),
         //   unsigned(dCurrent), unsigned(dDuration), bPlaying ? 1 : 0);
      }
   }
   else
      if (wParam == IDT_MOUSE_MESSAGE_CHECK)
      {
         ATLTRACE(_T("timer elapsed\n"));

         // no mouse moved for some time; toggle fullscreen
         if (!IsFullscreen(m_hWnd))
         {
            m_uiIgnoreNextMouseMessageCount = 1;

            ATLTRACE(_T("go to fullscreen\n"));

            SwitchToFullscreen();
            ToggleFullscreen(m_hWnd);
         }

         KillTimer(IDT_MOUSE_MESSAGE_CHECK);
      }

   return 0;
}

LRESULT MainFrame::OnActivate(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   if (wParam == WA_ACTIVE || wParam == WA_CLICKACTIVE)
   {
      // activated
      ATLTRACE(_T("activated\n"));

      // restart timer
      KillTimer(IDT_MOUSE_MESSAGE_CHECK);
      SetTimer(IDT_MOUSE_MESSAGE_CHECK, c_uiAutoFullscreenInSeconds * 1000);
   }
   else
      if (wParam == WA_INACTIVE)
      {
         ATLTRACE(_T("deactivated\n"));

         // stop timer
         KillTimer(IDT_MOUSE_MESSAGE_CHECK);
      }
   return 0;
}

LRESULT MainFrame::OnPlayStartPause(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   if (m_videoPlaybackManager.GetPlaybackState() == VideoPlaybackManager::stateStopped)
   {
      m_videoPlaybackManager.Start();
      m_screensaverState.Disable(); // now playing
      m_controlBar.SetStartPauseButton(false);
      m_lcPlaylist.SetStarted();

      if (m_scpTaskbar->IsAvailable())
      {
         m_scpTaskbarProgressBar.reset(new Win32::TaskbarProgressBar(m_scpTaskbar->OpenProgressBar()));
         m_scpTaskbarProgressBar->SetState(Win32::TaskbarProgressBar::TBPF_NORMAL);
         m_scpTaskbarProgressBar->SetPos(0, 100);
      }
   }
   else
   {
      m_videoPlaybackManager.Pause();
      if (m_videoPlaybackManager.GetPlaybackState() == VideoPlaybackManager::statePaused)
      {
         m_screensaverState.Enable(); // now stopped
         m_controlBar.SetStartPauseButton(true);

         if (m_scpTaskbarProgressBar != NULL)
            m_scpTaskbarProgressBar->SetState(Win32::TaskbarProgressBar::TBPF_PAUSED);
      }
      else
      {
         m_screensaverState.Disable(); // now playing
         m_controlBar.SetStartPauseButton(false);

         if (m_scpTaskbarProgressBar != NULL)
            m_scpTaskbarProgressBar->SetState(Win32::TaskbarProgressBar::TBPF_NORMAL);
      }
   }

   m_lastPlaybackState = m_videoPlaybackManager.GetPlaybackState();

   return 0;
}

LRESULT MainFrame::OnPlayNext(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   m_videoPlaybackManager.Crossfade();

   return 0;
}

LRESULT MainFrame::OnViewSearch(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   m_bViewSearch = !m_bViewSearch;

   m_controlBar.SetSearchButton(m_bViewSearch);

   UpdatePaneVisibility();

   return 0;
}

LRESULT MainFrame::OnViewPlaylist(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   m_bViewPlaylist = !m_bViewPlaylist;

   m_controlBar.SetPlaylistButton(m_bViewPlaylist);

   UpdatePaneVisibility();

   return 0;
}

void MainFrame::AddPlaybackPage()
{
   m_splitterPlayback.Create(m_splitterLower, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

   m_viewFirst.Create(m_splitterPlayback, rcDefault, _T("about:blank"), WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_HSCROLL | WS_VSCROLL, 0);
   m_viewSecond.Create(m_splitterPlayback, rcDefault, _T("about:blank"), WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_HSCROLL | WS_VSCROLL, 0);

   m_viewFirst.Init();
   m_viewSecond.Init();

   m_splitterPlayback.SetSplitterPanes(m_viewFirst, m_viewSecond);
#ifdef _DEBUG
   m_splitterPlayback.SetSinglePaneMode(SPLIT_PANE_NONE);
#else
   // in release, start up with only the first pane
   m_splitterPlayback.SetSinglePaneMode(SPLIT_PANE_LEFT);
#endif
   m_splitterPlayback.SetSplitterPosPct(80);
}

void MainFrame::AddSearchPage()
{
   m_searchView.Create(m_splitterLower, rcDefault, _T("http://www.youtube.com/"), WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_HSCROLL | WS_VSCROLL, WS_EX_CLIENTEDGE);
}

void MainFrame::AddPlaylistPane()
{
   m_panePlaylist.SetPaneContainerExtendedStyle(PANECNT_NOBORDER | PANECNT_NOCLOSEBUTTON);
   m_panePlaylist.Create(m_splitterLower, _T("Playlist"), WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

   m_lcPlaylist.Create(m_panePlaylist, rcDefault);
   m_lcPlaylist.InitView();
   m_lcPlaylist.UpdatePlaylist();

   m_panePlaylist.SetClient(m_lcPlaylist);
}

void MainFrame::SwitchToFullscreen()
{
   // hide playlist and search
   m_bViewPlaylist = false;
   m_bViewSearch = false;
   UpdatePaneVisibility();

   // hide pane with buttons
   m_splitterAll.SetSinglePaneMode(SPLIT_PANE_RIGHT);

   // hide menu
   m_hHiddenMenu = GetMenu();
   SetMenu(NULL);
}

void MainFrame::SwitchToWindowed()
{
   m_splitterAll.SetSinglePaneMode(SPLIT_PANE_NONE);

   SetMenu(m_hHiddenMenu);
}

void MainFrame::UpdatePaneVisibility()
{
   HWND hWndLeftWindow;
   if (m_bViewSearch)
   {
      hWndLeftWindow = m_searchView;
      m_searchView.ShowWindow(SW_SHOW);
      m_splitterPlayback.ShowWindow(SW_HIDE);
   }
   else
   {
      hWndLeftWindow = m_splitterPlayback;
      m_splitterPlayback.ShowWindow(SW_SHOW);
      m_searchView.ShowWindow(SW_HIDE);
   }

   if (!m_bViewPlaylist)
   {
      m_splitterLower.SetSplitterPane(SPLIT_PANE_LEFT, hWndLeftWindow, true);
      m_splitterLower.SetSinglePaneMode(SPLIT_PANE_LEFT);
   }
   else
   {
      m_splitterLower.SetSplitterPanes(hWndLeftWindow, m_panePlaylist);
      m_splitterLower.SetSinglePaneMode(SPLIT_PANE_NONE);
   }
}

void MainFrame::OnAddTitle(const CString& cszTitle, const CString& cszAddress)
{
   m_tempPlaylistProvider.AddVideo(cszTitle, cszAddress);
   m_lcPlaylist.UpdatePlaylist();
}

void MainFrame::OnSearchTitle(const CString& cszText)
{
   // switch to search view
   m_bViewSearch = true;
   m_controlBar.SetSearchButton(m_bViewSearch);
   UpdatePaneVisibility();

   // start search in view
   m_searchView.SearchVideo(cszText);
}

LRESULT MainFrame::OnAppExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   PostMessage(WM_CLOSE);
   return 0;
}

LRESULT MainFrame::OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   AboutDlg dlg;
   dlg.DoModal();
   return 0;
}

void MainFrame::SetFrameTitle(bool bIsPlaying, const CString& cszTitle)
{
   CString cszText;
   if (bIsPlaying)
      cszText.Format(_T("PartyVideoDJ playing \"%s\""), cszTitle.GetString());
   else
      cszText = _T("PartyVideoDJ");

   SetWindowText(cszText);
}

YouTubeWebPageView& MainFrame::GetPlaybackView()
{
   return m_fViewFirstIsPlayback ? m_viewFirst : m_viewSecond;
}

YouTubeWebPageView& MainFrame::GetPreloadView()
{
   return m_fViewFirstIsPlayback ? m_viewSecond : m_viewFirst;
}

void MainFrame::StartTicking()
{
   m_lastPlaybackState = m_videoPlaybackManager.GetPlaybackState();
   SetTimer(IDT_PLAYBACK_TICK, c_uiTickTime);
}

void MainFrame::StopTicking()
{
   KillTimer(IDT_PLAYBACK_TICK);
}

void MainFrame::SetupPane(bool bPlaybackView, const VideoInfo& info)
{
   YouTubeWebPageView& view = bPlaybackView ? GetPlaybackView() : GetPreloadView();
   view.LoadVideo(info);

   m_splitterPlayback.UpdateSplitterLayout();
}

void MainFrame::EmptyPane(bool bPlaybackView)
{
   YouTubeWebPageView& view = bPlaybackView ? GetPlaybackView() : GetPreloadView();
   view.StopVideo();
   view.ClearVideo();
   view.SetActionWhenReady(YouTubeWebPageView::actionDoNothing);

   m_splitterPlayback.UpdateSplitterLayout();
}

void MainFrame::SwapPanes()
{
   // switch views
   m_fViewFirstIsPlayback = !m_fViewFirstIsPlayback;
   m_splitterPlayback.SetSplitterPanes(m_splitterPlayback.GetSplitterPane(SPLIT_PANE_RIGHT), m_splitterPlayback.GetSplitterPane(SPLIT_PANE_LEFT));

   // in single-pane mode? ensure proper pane is shown
   if (m_splitterPlayback.GetSinglePaneMode() != SPLIT_PANE_NONE)
      m_splitterPlayback.SetSinglePaneMode(SPLIT_PANE_LEFT);
}

void MainFrame::OnUpdatePlaybackVideo(const CString& cszNewVideoName)
{
   // update playback video name
   // video is playing when video name is not empty
   SetFrameTitle(!cszNewVideoName.IsEmpty(), cszNewVideoName);

   // update playlist view
   m_lcPlaylist.UpdatePlaylist();

   m_logFile.LogSong(cszNewVideoName);
}
