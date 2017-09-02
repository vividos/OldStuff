//
// PartyVideoDJ - YouTube crossfading app
// Copyright (C) 2008-2017 Michael Fink
//
/// \file PlaylistCtrl.hpp Playlist control
//
#pragma once

#include "ListViewDragRearrange.h"

class TemporaryPlaylistProvider;

typedef CWinTraitsOR<
   LVS_REPORT | LVS_SHOWSELALWAYS | LVS_SINGLESEL | LVS_NOSORTHEADER, 0> CPlaylistCtrlTraits;

class PlaylistCtrl :
   public CWindowImpl<PlaylistCtrl, CListViewCtrl, CPlaylistCtrlTraits>,
   public CCustomDraw<PlaylistCtrl>,
   public CListViewDragRearrangeT<PlaylistCtrl>
{
public:
   PlaylistCtrl(TemporaryPlaylistProvider& provider);
   ~PlaylistCtrl() throw() {}

   void InitView();
   void UpdatePlaylist();

   /// notifies ctrl that playback has been started
   void SetStarted() throw() { m_bStarted = true; }

   DECLARE_WND_SUPERCLASS(_T("PartyVideoDJ_PlaylistCtrl"), CListViewCtrl::GetWndClassName())

private:
   BEGIN_MSG_MAP(PlaylistCtrl)
      REFLECTED_NOTIFY_CODE_HANDLER(NM_RCLICK, OnRightClick)
      COMMAND_ID_HANDLER(IDC_PLAYLIST_DELETE, OnDelete)
      COMMAND_ID_HANDLER(IDC_PLAYLIST_MOVE_DOWN, OnMoveDown)
      COMMAND_ID_HANDLER(IDC_PLAYLIST_MOVE_UP, OnMoveUp)
      COMMAND_ID_HANDLER(IDC_PLAYLIST_SET_NEXT, OnSetNext)
      MESSAGE_HANDLER(WM_KEYUP, OnKeyUp)
      CHAIN_MSG_MAP_ALT(CCustomDraw<PlaylistCtrl>, 1)
      CHAIN_MSG_MAP_ALT(CListViewDragRearrangeT<PlaylistCtrl>, 1)
   END_MSG_MAP()

   DWORD OnPrePaint(int /*idCtrl*/, LPNMCUSTOMDRAW /*lpNMCustomDraw*/)
   {
      return CDRF_NOTIFYITEMDRAW | CDRF_NOTIFYPOSTPAINT;
   }

   DWORD OnItemPrePaint(int /*idCtrl*/, LPNMCUSTOMDRAW lpNMCustomDraw)
   {
      LPNMLVCUSTOMDRAW lpNMLVCustomDraw = reinterpret_cast<LPNMLVCUSTOMDRAW>(lpNMCustomDraw);
      DWORD_PTR dwItem = lpNMCustomDraw->dwItemSpec;

      // color every other line
      if ((dwItem & 1) && dwItem < static_cast<DWORD_PTR>(GetItemCount() - 1))
         lpNMLVCustomDraw->clrTextBk = RGB(0xf0, 0xf0, 0xf0);

      return CDRF_DODEFAULT;
   }

   // Handler prototypes (uncomment arguments if needed):
   // LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
   // LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
   // LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

   LRESULT OnRightClick(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/);
   LRESULT OnDelete(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
   LRESULT OnMoveDown(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
   LRESULT OnMoveUp(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
   LRESULT OnSetNext(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
   LRESULT OnKeyUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

private:
   void DeleteSelectedEntry();
   void DeleteTrackByIndex(size_t uiPos);

   friend CListViewDragRearrangeT<PlaylistCtrl>;

   void ItemMoved(int nOldIndex, int nNewIndex, LPARAM lParamItem);

private:
   TemporaryPlaylistProvider& m_provider;

   CMenu m_menuContext;
   CImageList m_ilIcons;
   CFont m_fontList;

   bool m_bStarted;
};
