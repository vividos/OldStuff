//
// PartyVideoDJ - YouTube crossfading app
// Copyright (C) 2008-2017 Michael Fink
//
/// \file YouTubeWebPageView.cpp View to display YouTube web page
//
#include "StdAfx.h"
#include "YouTubeWebPageView.hpp"
#include "VideoInfo.hpp"
#include <ulib/win32/DocHostUI.hpp>
#include <ulib/win32/ResourceData.hpp>
#include "resource.h" // for IDR_HTML_TEMPLATE_YOUTUBE

#define IDT_SEEK_AND_PAUSE 101

YouTubeWebPageView::YouTubeWebPageView() throw()
   :m_enActionWhenReady(actionDoNothing),
   m_dSeconds(0.0),
   m_uiSeekAndPauseStopCounter(0),
   m_iLastError(0),
   m_bLoaded(false)
{
}

YouTubeWebPageView::~YouTubeWebPageView() throw()
{
   DeleteFile(m_cszHtmlFilename);
   m_cszHtmlFilename.Empty();
}

BOOL YouTubeWebPageView::PreTranslateMessage(MSG* pMsg)
{
   if ((pMsg->message >= WM_KEYFIRST && pMsg->message <= WM_KEYLAST) &&
      (pMsg->message >= WM_MOUSEFIRST && pMsg->message <= WM_MOUSELAST))
      return TRUE;

   // give HTML page a chance to translate this message
   return WebPageWindowEx::PreTranslateMessage(pMsg);;
}

void YouTubeWebPageView::PlayVideo()
{
   if (!m_bLoaded) return;

   ExecuteJavaScript(_T("play()"));

   ATLTRACE(_T("%08x: PlayVideo()\n"), m_hWnd);
}

void YouTubeWebPageView::PauseVideo()
{
   if (!m_bLoaded) return;

   ExecuteJavaScript(_T("pause()"));

   ATLTRACE(_T("%08x: PauseVideo()\n"), m_hWnd);
}

void YouTubeWebPageView::StopVideo()
{
   if (!m_bLoaded) return;

   ExecuteJavaScript(_T("stopVideo()"));

   ATLTRACE(_T("%08x: StopVideo()\n"), m_hWnd);
}

void YouTubeWebPageView::ClearVideo()
{
   if (!m_bLoaded) return;

   ExecuteJavaScript(_T("clearVideo()"));

   ATLTRACE(_T("%08x: ClearVideo()\n"), m_hWnd);
}

unsigned int YouTubeWebPageView::GetVideoBytesLoaded()
{
   CComVariant varResult;
   CallFunction(_T("getVideoBytesLoaded"), std::vector<CComVariant>(), varResult);

   ATLASSERT(varResult.vt == VT_I4);
   return varResult.intVal;
}

unsigned int YouTubeWebPageView::GetVideoBytesTotal()
{
   CComVariant varResult;
   CallFunction(_T("getVideoBytesTotal"), std::vector<CComVariant>(), varResult);

   ATLASSERT(varResult.vt == VT_I4);
   return varResult.intVal;
}

void YouTubeWebPageView::Mute()
{
   if (!m_bLoaded) return;

   ExecuteJavaScript(_T("mute()"));
}

void YouTubeWebPageView::Unmute()
{
   if (!m_bLoaded) return;

   ExecuteJavaScript(_T("unmute()"));
}

bool YouTubeWebPageView::IsMuted()
{
   CComVariant varResult;
   CallFunction(_T("isMuted"), std::vector<CComVariant>(), varResult);

   ATLASSERT(varResult.vt == VT_BOOL);
   return varResult.boolVal == VARIANT_TRUE;
}

void YouTubeWebPageView::SetVolume(unsigned int uiVolume)
{
   if (IsPlayerError())
      return; // ignore volume changes on error

   std::vector<CComVariant> vecArgs;

   CComVariant varVolume(static_cast<int>(uiVolume));
   vecArgs.push_back(varVolume);

   CComVariant varResult;
   CallFunction(_T("setVolume"), vecArgs, varResult);

   ATLTRACE(_T("%08x: SetVolume(%u)\n"),
      m_hWnd,
      uiVolume);
}

unsigned int YouTubeWebPageView::GetVolume()
{
   CComVariant varResult;
   CallFunction(_T("getVolume"), std::vector<CComVariant>(), varResult);

   ATLASSERT(varResult.vt == VT_I4);
   return varResult.intVal;
}

void YouTubeWebPageView::SeekTo(double dSeconds, bool bAllowSeekAhead)
{
   std::vector<CComVariant> vecArgs;

   // note: we have to push back arguments in reverse order
   CComVariant varAllowSeekAhead(bAllowSeekAhead);
   vecArgs.push_back(varAllowSeekAhead);

   CComVariant varSeconds(dSeconds);
   vecArgs.push_back(varSeconds);

   CComVariant varResult;
   CallFunction(_T("seekTo"), vecArgs, varResult);

   ATLTRACE(_T("%08x: SeekTo(%i)\n"),
      m_hWnd,
      static_cast<int>(dSeconds));
}

YouTubeWebPageView::T_enPlayerState YouTubeWebPageView::GetPlayerState()
{
   if (IsPlayerError())
      return playerStateUnknown;

   CComVariant varResult;
   CallFunction(_T("getPlayerState"), std::vector<CComVariant>(), varResult);

   if (varResult.vt != VT_I4)
      return playerStateUnknown;

   return static_cast<T_enPlayerState>(varResult.intVal);
}

double YouTubeWebPageView::GetCurrentTime()
{
   T_enPlayerState enState = GetPlayerState();
   if (enState != playerStateVideoCued &&
      enState != playerStatePlaying &&
      enState != playerStateBuffering)
      return 0.0;

   CComVariant varResult;
   CallFunction(_T("getCurrentTime"), std::vector<CComVariant>(), varResult);

   if (varResult.vt != VT_R8)
      return 0.0;

   return varResult.dblVal;
}

double YouTubeWebPageView::GetDuration()
{
   T_enPlayerState enState = GetPlayerState();
   if (enState != playerStateVideoCued &&
      enState != playerStatePlaying &&
      enState != playerStateBuffering)
      return 1.0;

   CComVariant varResult;
   CallFunction(_T("getDuration"), std::vector<CComVariant>(), varResult);

   if (varResult.vt != VT_R8)
      return -1.0;

   return varResult.dblVal;
}

void YouTubeWebPageView::SetSize(int iWidth, int iHeight)
{
   std::vector<CComVariant> vecArgs;

   // note: we have to push back arguments in reverse order
   CComVariant varHeight(iHeight);
   vecArgs.push_back(varHeight);

   CComVariant varWidth(iWidth);
   vecArgs.push_back(varWidth);

   CComVariant varResult;
   CallFunction(_T("setSize"), vecArgs, varResult);
}

LRESULT YouTubeWebPageView::OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
{
   int iWidth = LOWORD(lParam);
   int iHeight = HIWORD(lParam);

   SetSize(iWidth, iHeight);

   bHandled = false;
   return 0;
}

LRESULT YouTubeWebPageView::OnTimer(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   if (wParam == IDT_SEEK_AND_PAUSE)
      CheckSeekToStart();

   return 0;
}

CString YouTubeWebPageView::GetStateCodeText(T_enPlayerState enPlayerState)
{
   switch (enPlayerState)
   {
   case playerStateUnknown:      return _T("Unknown");
   case playerStateInitialized:  return _T("Initialized");
   case playerStateUnstarted:    return _T("Unstarted");
   case playerStateEnded:        return _T("Ended");
   case playerStatePlaying:      return _T("Playing");
   case playerStatePaused:       return _T("Paused");
   case playerStateBuffering:    return _T("Buffering");
   case playerStateVideoCued:    return _T("VideoCued");
   }

   CString cszText;
   cszText.Format(_T("Unknown (%i)"), enPlayerState);
   return cszText;
}

CString YouTubeWebPageView::GetPlayerErrorText(T_enPlayerErrorCode enErrorCode)
{
   switch (enErrorCode)
   {
   case errorVideoNotFound:         return _T("Video not found");
   case errorPlaybackNotAllowed:    return _T("Playback not allowed (1)");
   case errorPlaybackNotAllowed2:   return _T("Playback not allowed (2)");
   }

   CString cszText;
   cszText.Format(_T("Unknown (%i)"), enErrorCode);
   return cszText;
}

LRESULT YouTubeWebPageView::OnStateChanged(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
   T_enPlayerState enState = static_cast<T_enPlayerState>(static_cast<int>(lParam));

   ATLTRACE(_T("%08x: OnStateChanged: %s (%i)\n"),
      m_hWnd,
      GetStateCodeText(enState).GetString(),
      lParam);

   if (enState == playerStateInitialized)
   {
      // simulate a WM_SIZE message to adjust web page size
      CRect rc;
      GetClientRect(&rc);

      BOOL bHandled = false;
      OnSize(WM_SIZE, 0, MAKELPARAM(rc.Width(), rc.Height()), bHandled);
   }
   else
      if (enState == playerStateVideoCued)
      {
         switch (m_enActionWhenReady)
         {
         case actionPlayVideo:
            SetVolume(100);
            PlayVideo();
            break;

         case actionSeekAndPause:
            m_uiSeekAndPauseStopCounter = 2;
            SetVolume(0);
            // note: calling SeekTo starts playing, so we pause the video when we reach the exact second
            SeekTo(m_dSeconds, true);
            break;

         case actionDoNothing:
         default:
            break;
         }
      }
      else
         if (enState == playerStatePlaying) // playing
         {
            // started playing due to SeekTo() call?
            if (m_enActionWhenReady == actionSeekAndPause)
            {
               CheckSeekToStart();
               SetTimer(IDT_SEEK_AND_PAUSE, 200);
            }
         }
         else
            if (enState == playerStatePaused) // paused
            {
               KillTimer(IDT_SEEK_AND_PAUSE);
               m_enActionWhenReady = actionDoNothing;
            }

   return 0;
}

LRESULT YouTubeWebPageView::OnPlayerError(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
   T_enPlayerErrorCode enErrorCode = static_cast<T_enPlayerErrorCode>(static_cast<int>(lParam));

   ATLTRACE(_T("%08x: OnPlayerError: %s (%i)\n"),
      m_hWnd,
      GetPlayerErrorText(enErrorCode).GetString(),
      lParam);

   m_iLastError = enErrorCode;

   return 0;
}

void YouTubeWebPageView::CheckSeekToStart()
{
   if (IsPlayerError())
      return; // ignore state changes when already in error state

   T_enPlayerState enState = GetPlayerState();
   if (playerStateUnknown == enState)
      return;

   ATLASSERT(enState == playerStateVideoCued || enState == playerStatePlaying || enState == playerStateBuffering);

   // pause video when at correct time index
   if (GetCurrentTime() >= m_dSeconds && enState == playerStatePlaying)
      PauseVideo();
}

void YouTubeWebPageView::LoadVideo(const VideoInfo& info)
{
   m_bLoaded = false;

   ATLTRACE(_T("%08x: LoadVideo: \"%s\" (%s)\n"),
      m_hWnd,
      info.Name().GetString(),
      info.Address().GetString());

   m_iLastError = 0;

   CString cszHtml = GetHtmlTemplate();

   cszHtml.Replace(_T("{..$address..}"), info.Address());
   cszHtml.Replace(_T("{..$name..}"), info.Name());
   cszHtml.Replace(_T("\r\n"), _T("\n"));

   // generate temp name
   CString cszTempFolder;
   GetTempPath(MAX_PATH, cszTempFolder.GetBuffer(MAX_PATH));
   cszTempFolder.ReleaseBuffer();

   CString cszFilename;
   GetTempFileName(cszTempFolder, _T("YTP"), 0, cszFilename.GetBuffer(MAX_PATH));
   cszFilename.ReleaseBuffer();

   // write to temporary file
   {
      FILE* fd = NULL;
      errno_t err = _tfopen_s(&fd, cszFilename, _T("wt"));
      ATLVERIFY(err == 0 && fd != NULL);

      USES_CONVERSION;
      fprintf(fd, T2CA(cszHtml));
      fclose(fd);
   }

   // navigate to page
   CComPtr<IWebBrowser2> spWebBrowser2;
   HRESULT hr = GetWebBrowser2(spWebBrowser2);
   if (SUCCEEDED(hr))
   {
      CString cszURL = _T("file:///") + cszFilename;
      cszURL.Replace(_T("\\"), _T("/"));

      CComBSTR bstrURL = cszURL;

      CComVariant varFlags(static_cast<int>(navNoHistory | navNoWriteToCache));

      hr = spWebBrowser2->Navigate(bstrURL, &varFlags, NULL, NULL, NULL);
   }

   if (!m_cszHtmlFilename.IsEmpty())
      DeleteFile(m_cszHtmlFilename);
   m_cszHtmlFilename = cszFilename;
}

CString YouTubeWebPageView::s_cszHtmlTemplate;

CString YouTubeWebPageView::GetHtmlTemplate() throw()
{
   if (s_cszHtmlTemplate.IsEmpty())
   {
      Win32::ResourceData rd(MAKEINTRESOURCE(IDR_HTML_TEMPLATE_YOUTUBE), RT_HTML);
      if (rd.IsAvailable())
         s_cszHtmlTemplate = rd.AsString(false); // not stored as unicode
   }

   return s_cszHtmlTemplate;
}
