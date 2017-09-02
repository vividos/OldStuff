//
// PartyVideoDJ - YouTube crossfading app
// Copyright (C) 2008-2017 Michael Fink
//
/// \file Playlist.cpp Playlist class
//
#include "stdafx.h"
#include "Playlist.hpp"
#include <algorithm>
#include <boost/bind.hpp>

void Playlist::Move(const std::vector<size_t>& vecEntryPosToMove, size_t uiInsertPos) throw()
{
   ATLASSERT(!vecEntryPosToMove.empty());

   // assert sorted list
   std::vector<size_t> vecSortedEntryPosToMove(vecEntryPosToMove.begin(), vecEntryPosToMove.end());
   std::sort(vecSortedEntryPosToMove.begin(), vecSortedEntryPosToMove.end());
   ATLASSERT(vecSortedEntryPosToMove.size() == vecEntryPosToMove.size());
   ATLASSERT(0 == memcmp(&vecSortedEntryPosToMove[0], &vecEntryPosToMove[0], vecSortedEntryPosToMove.size()));

   size_t uiMinPos = vecSortedEntryPosToMove.front();
   size_t uiMaxPos = vecSortedEntryPosToMove.back();

   ATLASSERT(uiInsertPos < uiMinPos || uiInsertPos > uiMaxPos); // insert point must lie outside of range

   if (uiInsertPos < uiMinPos)
      MoveUp(vecEntryPosToMove, uiInsertPos);
   else
      if (uiInsertPos > uiMaxPos)
         MoveDown(vecEntryPosToMove, uiInsertPos);
      else
         ATLVERIFY(false);

   // sort by sort_nr
   std::sort(m_vecPlaylistEntries.begin(), m_vecPlaylistEntries.end(),
      boost::bind(&PlaylistEntry::m_iSortNr, _1) < boost::bind(&PlaylistEntry::m_iSortNr, _2));
}

void Playlist::MoveUp(const std::vector<size_t>& vecEntryPosToMove, size_t uiInsertPos) throw()
{
   // adjust current and next; check every item
   for (size_t i = 0, iMax = vecEntryPosToMove.size(); i < iMax; i++)
   {
      if (vecEntryPosToMove[i] > m_uiCurrentPos && uiInsertPos < m_uiCurrentPos)
         m_uiCurrentPos++;

      if (vecEntryPosToMove[i] > m_uiNextPos && uiInsertPos < m_uiNextPos)
         m_uiNextPos++;
   }

   // insert all entries at new position
   for (size_t i = 0, iMax = vecEntryPosToMove.size(); i < iMax; i++)
   {
      // insert at new position
      size_t uiPosToMove = vecEntryPosToMove[i] + i; // add already moved item count
      m_vecPlaylistEntries.insert(
         m_vecPlaylistEntries.begin() + uiInsertPos + i,
         m_vecPlaylistEntries[uiPosToMove]);
   }

   // now delete all original locations, last to first
   size_t uiOffset = vecEntryPosToMove.size();
   for (size_t i = vecEntryPosToMove.size(); i > 0; i--)
   {
      size_t uiPosToDelete = vecEntryPosToMove[i - 1] + uiOffset;

      m_vecPlaylistEntries.erase(m_vecPlaylistEntries.begin() + uiPosToDelete);
   }
}

void Playlist::MoveDown(const std::vector<size_t>& vecEntryPosToMove, size_t uiInsertPos) throw()
{
   // adjust current and next; check every item
   for (size_t i = 0, iMax = vecEntryPosToMove.size(); i < iMax; i++)
   {
      if (vecEntryPosToMove[i] < m_uiCurrentPos && uiInsertPos > m_uiCurrentPos)
         m_uiCurrentPos--;

      if (vecEntryPosToMove[i] < m_uiNextPos && uiInsertPos > m_uiNextPos)
         m_uiNextPos--;
   }

   // insert all entries at new position
   for (size_t i = 0, iMax = vecEntryPosToMove.size(); i < iMax; i++)
   {
      // insert at new position
      size_t uiPosToMove = vecEntryPosToMove[i];
      m_vecPlaylistEntries.insert(
         m_vecPlaylistEntries.begin() + uiInsertPos + i + 1,
         m_vecPlaylistEntries[uiPosToMove]);
   }

   // now delete all original locations, last to first
   for (size_t i = vecEntryPosToMove.size(); i > 0; i--)
   {
      size_t uiPosToDelete = vecEntryPosToMove[i - 1];

      m_vecPlaylistEntries.erase(m_vecPlaylistEntries.begin() + uiPosToDelete);
   }
}
