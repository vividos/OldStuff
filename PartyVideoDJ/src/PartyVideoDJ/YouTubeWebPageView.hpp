//
// PartyVideoDJ - YouTube crossfading app
// Copyright (C) 2008-2017 Michael Fink
//
/// \file YouTubeWebPageView.hpp View to display YouTube web page
//
#pragma once

#include "WebPageWindowEx.hpp"

class VideoInfo;

const DISPID c_uiExternalStateId = 0;
const DISPID c_uiExternalErrorId = 1;

template <typename T>
class YouTubePlayerExternalImpl :
   public IDocHostUIHandlerDispatchImpl,
   public IOleCommandTarget
{
public:
   virtual ULONG STDMETHODCALLTYPE AddRef() override { return IDocHostUIHandlerDispatchImpl::AddRef(); }

   virtual ULONG STDMETHODCALLTYPE Release() override { return IDocHostUIHandlerDispatchImpl::Release(); }

   virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject) override
   {
      if (riid == IID_IOleCommandTarget)
      {
         if (!ppvObject)
            return E_POINTER;

         *ppvObject = static_cast<IOleCommandTarget*>(this);
         AddRef();
         return S_OK;
      }

      return IDocHostUIHandlerDispatchImpl::QueryInterface(riid, ppvObject);
   }

   virtual /* [input_sync] */ HRESULT STDMETHODCALLTYPE QueryStatus(
      const GUID* /*pguidCmdGroup*/,
      ULONG /*cCmds*/,
      OLECMD /*prgCmds*/[],
      OLECMDTEXT* /*pCmdText*/) override
   {
      return E_NOTIMPL;
   }

   virtual HRESULT STDMETHODCALLTYPE Exec(
      const GUID* /*pguidCmdGroup*/,
      DWORD /*nCmdID*/,
      DWORD /*nCmdexecopt*/,
      VARIANT* /*pvaIn*/,
      VARIANT* /*pvaOut*/) override
   {
      return E_NOTIMPL;
   }

   // reimplement some methods from IDispatch
   /// returns IDs by given names
   virtual HRESULT STDMETHODCALLTYPE GetIDsOfNames(const IID&, LPOLESTR* pszoName, UINT cNames,
      LCID /*lcid*/, DISPID* rgdispid) override
   {
      // this function assumes the host always queries only for one name
      ATLASSERT(cNames == 1); cNames;
      CString cszName(*pszoName);

      if (cszName == _T("state"))
      {
         rgdispid[0] = c_uiExternalStateId;
         return S_OK;
      }
      else
         if (cszName == _T("error"))
         {
            rgdispid[0] = c_uiExternalErrorId;
            return S_OK;
         }

      return E_FAIL;
   }

   /// calls IDispatch methods
   virtual HRESULT STDMETHODCALLTYPE Invoke(
      DISPID dispid, REFIID /*riid*/, LCID /*lcid*/, WORD wFlags,
      DISPPARAMS* pDispParams, VARIANT* /*pVarResult*/,
      EXCEPINFO* /*pExcepInfo*/, UINT* /*puArgErr*/) override
   {
      if (dispid == c_uiExternalStateId)
      {
         CComVariant varValue(*pDispParams->rgvarg);

         if (wFlags == DISPATCH_PROPERTYPUT)
         {
            T* pT = static_cast<T*>(this);
            pT->OnChangedState(varValue.intVal);
         }

         return S_OK;
      }

      if (dispid == c_uiExternalErrorId)
      {
         CComVariant varValue(*pDispParams->rgvarg);

         if (wFlags == DISPATCH_PROPERTYPUT)
         {
            T* pT = static_cast<T*>(this);
            pT->OnPlayerError(varValue.intVal);
         }

         return S_OK;
      }

      return E_FAIL;
   }

   // *** IDocHostUIHandlerDispatchImpl ***
   virtual HRESULT STDMETHODCALLTYPE GetExternal(IDispatch** ppDispatch) override
   {
      *ppDispatch = this;
      AddRef();
      return S_OK;
   }

   // non-virtual functions

   void OnChangedState(int iState)
   {
      ATLASSERT(false);
   }

   void OnPlayerError(int iError)
   {
      ATLASSERT(false);
   }
};

#define WM_STATE_CHANGED WM_APP + 1
#define WM_PLAYER_ERROR WM_APP + 2

class YouTubeWebPageView :
   public WebPageWindowEx<YouTubeWebPageView, YouTubePlayerExternalImpl<YouTubeWebPageView> >
{
public:
   typedef WebPageWindowEx<YouTubeWebPageView, YouTubePlayerExternalImpl<YouTubeWebPageView> > BaseClass;

   YouTubeWebPageView() throw();
   ~YouTubeWebPageView() throw();

   BOOL PreTranslateMessage(MSG* pMsg);

   void OnNewWindow3(const CString& /*cszURL*/, bool& bCancel) const
   {
      bCancel = true; // cancel all newly opened windows
   }


   enum T_enActionWhenReady
   {
      actionDoNothing = 0,
      actionPlayVideo = 1, ///< immediately start playing video when it's cued
      actionSeekAndPause = 2, ///< seek forward to position and pause
   };

   void SetActionWhenReady(T_enActionWhenReady enActionWhenReady, double dSeconds = 0.0) throw()
   {
      m_enActionWhenReady = enActionWhenReady;
      m_dSeconds = dSeconds;
   }

   enum T_enPlayerState
   {
      playerStateUnknown = -3,
      playerStateInitialized = -2, ///< state set when onYouTubePlayerReady was called
      playerStateUnstarted = -1,
      playerStateEnded = 0,
      playerStatePlaying = 1,
      playerStatePaused = 2,
      playerStateBuffering = 3,
      playerStateVideoCued = 5, ///< video download not yet started, showing "play" button on the video
   };

   enum T_enPlayerErrorCode
   {
      errorVideoNotFound = 100, ///< video removed or marked as private
      errorPlaybackNotAllowed = 101, ///< video isn't allowed to be embedded
      errorPlaybackNotAllowed2 = 150, ///< same, but different code
   };

   static CString GetStateCodeText(T_enPlayerState enPlayerState);
   static CString GetPlayerErrorText(T_enPlayerErrorCode enErrorCode);

   void LoadVideo(const VideoInfo& info);

   void PlayVideo();
   void PauseVideo();
   void StopVideo();
   void ClearVideo();

   unsigned int GetVideoBytesLoaded();
   unsigned int GetVideoBytesTotal();

   void Mute();
   void Unmute();
   bool IsMuted();

   void SetVolume(unsigned int uiVolume);
   unsigned int GetVolume();

   void SeekTo(double dSeconds, bool bAllowSeekAhead);

   T_enPlayerState GetPlayerState();

   double GetCurrentTime();
   double GetDuration();
   void SetSize(int iWidth, int iHeight);

private:
   BEGIN_MSG_MAP(YouTubeWebPageView)
      MESSAGE_HANDLER(WM_SIZE, OnSize)
      MESSAGE_HANDLER(WM_TIMER, OnTimer)
      MESSAGE_HANDLER(WM_STATE_CHANGED, OnStateChanged)
      MESSAGE_HANDLER(WM_PLAYER_ERROR, OnPlayerError)
      MESSAGE_HANDLER(WM_LBUTTONDOWN, OnIgnoreMessage)
      MESSAGE_HANDLER(WM_LBUTTONUP, OnIgnoreMessage)
      MESSAGE_HANDLER(WM_LBUTTONDBLCLK, OnIgnoreMessage)
      MESSAGE_HANDLER(WM_RBUTTONDOWN, OnIgnoreMessage)
      MESSAGE_HANDLER(WM_RBUTTONUP, OnIgnoreMessage)
      MESSAGE_HANDLER(WM_RBUTTONDBLCLK, OnIgnoreMessage)
      MESSAGE_RANGE_HANDLER(WM_MOUSEFIRST, WM_MOUSELAST, OnIgnoreMessage)
      CHAIN_MSG_MAP(BaseClass)
   END_MSG_MAP()

   LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
   LRESULT OnTimer(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
   LRESULT OnStateChanged(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
   LRESULT OnPlayerError(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

   LRESULT OnIgnoreMessage(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
   {
      bHandled = TRUE;
      return 0;
   }

   void CheckSeekToStart();

   bool IsPlayerError() const { return m_iLastError != 0; }

   static CString GetHtmlTemplate() throw();


   // non-virtual functions

   void OnChangedState(int iState)
   {
      m_bLoaded = true;
      ::PostMessage(m_hWnd, WM_STATE_CHANGED, 0, static_cast<LPARAM>(iState));
   }

   void OnPlayerError(int iError)
   {
      ::PostMessage(m_hWnd, WM_PLAYER_ERROR, 0, static_cast<LPARAM>(iError));
   }

private:
   T_enActionWhenReady m_enActionWhenReady;
   double m_dSeconds;

   unsigned int m_uiSeekAndPauseStopCounter;

   int m_iLastError;

   bool m_bLoaded;

   CString m_cszHtmlFilename;

   static CString s_cszHtmlTemplate;
};
