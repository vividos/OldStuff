//
// PartyVideoDJ - YouTube crossfading app
// Copyright (C) 2008-2017 Michael Fink
//
/// \file PlaylistCtrl.cpp Playlist control
//
#include "StdAfx.h"
#include "resource.h"
#include "PlaylistCtrl.hpp"
#include "IconList.hpp"
#include "TemporaryPlaylistProvider.hpp"

PlaylistCtrl::PlaylistCtrl(TemporaryPlaylistProvider& provider)
   :m_provider(provider),
   m_bStarted(false)
{
   m_menuContext.LoadMenu(IDR_MENU_CTX_PLAYLIST);
}

void PlaylistCtrl::InitView()
{
   LoadIconList(m_ilIcons);
   SetImageList(m_ilIcons, LVSIL_SMALL);
   SetImageList(m_ilIcons, LVSIL_NORMAL);

   SetExtendedListViewStyle(LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);

   InsertColumn(0, _T("Title"));

   // set font
   CFontHandle defaultFont(AtlGetDefaultGuiFont());
   LOGFONT logFont;
   defaultFont.GetLogFont(&logFont);

   logFont.lfHeight = LONG(logFont.lfHeight * 1.7);

   m_fontList.CreateFontIndirect(&logFont);
   SetFont(m_fontList);
}

void PlaylistCtrl::UpdatePlaylist()
{
   SetRedraw(FALSE);
   DeleteAllItems();

   std::shared_ptr<IPlaylist> spIPlaylist = m_provider.GetCurrentPlaylist();
   std::shared_ptr<Playlist> spPlaylist = std::dynamic_pointer_cast<Playlist>(spIPlaylist);
   Playlist& playlist = *spPlaylist;

   size_t uiCurrentPos = playlist.GetCurrentPos();
   size_t uiNextPos = playlist.GetNextPos();
   int iPlaybackIndex = -1;

   for (size_t i = 0, iMax = playlist.GetSize(); i < iMax; i++)
   {
      const PlaylistEntry& entry = playlist.GetEntry(i);
      const VideoInfo& info = m_provider.GetVideoInfo(entry.m_iId);

      bool bIsCurrent = i == uiCurrentPos;
      bool bIsNext = i == uiNextPos;

      T_enIcons enIcon = iconFilm;
      if (bIsCurrent)
         enIcon = iconArrow;
      if (bIsNext)
         enIcon = iconNext;
      if (entry.m_bHasError)
         enIcon = iconError;

      int iItem = InsertItem(static_cast<int>(i), info.Name(), enIcon);

      if (bIsCurrent)
         iPlaybackIndex = iItem;

      SetItemData(iItem, entry.m_iId);
   }

   // autosize last column
   SetColumnWidth(0, LVSCW_AUTOSIZE);

   // scroll into view
   if (iPlaybackIndex != -1)
      CListViewCtrl::EnsureVisible(iPlaybackIndex, FALSE);

   SetRedraw(TRUE);
}

void PlaylistCtrl::DeleteSelectedEntry()
{
   int iIndex = GetSelectedIndex();
   if (iIndex == -1)
      return;

   DeleteTrackByIndex(iIndex);

   CListViewCtrl::DeleteItem(iIndex);

   // re-select icon with same index
   int iMax = GetItemCount();
   if (iIndex >= iMax)
      iIndex = iMax - 1;

   UINT iState = LVIS_SELECTED | LVIS_FOCUSED;
   CListViewCtrl::SetItemState(iIndex, iState, iState);
}

LRESULT PlaylistCtrl::OnRightClick(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)
{
   CPoint pt;
   GetCursorPos(&pt);

   TrackPopupMenu(m_menuContext.GetSubMenu(0),
      TPM_RIGHTALIGN | TPM_LEFTBUTTON,
      pt.x, pt.y, 0, m_hWnd, NULL);

   return 0;
}

LRESULT PlaylistCtrl::OnDelete(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   DeleteSelectedEntry();
   return 0;
}

LRESULT PlaylistCtrl::OnMoveDown(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   int iIndex = GetSelectedIndex();
   if (iIndex == -1)
      return 0;

   if (iIndex == GetItemCount() - 1)
      return 0; // last item

   std::shared_ptr<IPlaylist> spIPlaylist = m_provider.GetCurrentPlaylist();
   std::shared_ptr<Playlist> spPlaylist = std::dynamic_pointer_cast<Playlist>(spIPlaylist);
   Playlist& playlist = *spPlaylist;

   playlist.MoveOne(iIndex, iIndex + 1);

   UpdatePlaylist();

   return 0;
}

LRESULT PlaylistCtrl::OnMoveUp(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   int iIndex = GetSelectedIndex();
   if (iIndex == -1)
      return 0;

   if (iIndex == 0)
      return 0;

   std::shared_ptr<IPlaylist> spIPlaylist = m_provider.GetCurrentPlaylist();
   std::shared_ptr<Playlist> spPlaylist = std::dynamic_pointer_cast<Playlist>(spIPlaylist);
   Playlist& playlist = *spPlaylist;

   playlist.MoveOne(iIndex, iIndex - 1);

   UpdatePlaylist();

   return 0;
}

LRESULT PlaylistCtrl::OnSetNext(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   int iIndex = GetSelectedIndex();
   if (iIndex == -1)
      return 0;

   std::shared_ptr<IPlaylist> spIPlaylist = m_provider.GetCurrentPlaylist();
   std::shared_ptr<Playlist> spPlaylist = std::dynamic_pointer_cast<Playlist>(spIPlaylist);
   Playlist& playlist = *spPlaylist;

   if (m_bStarted)
      playlist.SetNextPos(iIndex);
   else
   {
      playlist.SetCurrentPos(iIndex);
      playlist.SetNextPos(iIndex + 1);
   }

   UpdatePlaylist();

   return 0;
}

LRESULT PlaylistCtrl::OnKeyUp(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   if (wParam == VK_DELETE)
      DeleteSelectedEntry();

   return 0;
}

void PlaylistCtrl::DeleteTrackByIndex(size_t uiPos)
{
   std::shared_ptr<IPlaylist> spIPlaylist = m_provider.GetCurrentPlaylist();
   std::shared_ptr<Playlist> spPlaylist = std::dynamic_pointer_cast<Playlist>(spIPlaylist);
   Playlist& playlist = *spPlaylist;

   playlist.RemoveAt(uiPos);
}

void PlaylistCtrl::ItemMoved(int nOldIndex, int nNewIndex, LPARAM /*lParamItem*/)
{
   if (nOldIndex == nNewIndex)
      return;

   std::shared_ptr<IPlaylist> spIPlaylist = m_provider.GetCurrentPlaylist();
   std::shared_ptr<Playlist> spPlaylist = std::dynamic_pointer_cast<Playlist>(spIPlaylist);
   Playlist& playlist = *spPlaylist;

   playlist.MoveOne(nOldIndex, nNewIndex);

   UpdatePlaylist();

   CListViewCtrl::EnsureVisible(nNewIndex, FALSE);
   CListViewCtrl::SelectItem(nNewIndex);
}
