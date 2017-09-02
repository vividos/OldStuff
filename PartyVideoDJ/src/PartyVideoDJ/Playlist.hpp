//
// PartyVideoDJ - YouTube crossfading app
// Copyright (C) 2008-2017 Michael Fink
//
/// \file Playlist.hpp Playlist class
//
#pragma once

#include <vector>
#include "IPlaylist.hpp"

struct PlaylistEntry
{
   PlaylistEntry() throw()
      :m_iId(-1),
      m_iVideoId(-1),
      m_iSortNr(-1),
      m_bHasError(false)
   {
   }

   int m_iId;
   T_VideoId m_iVideoId;
   int m_iSortNr;
   bool m_bHasError;
};

class Playlist : public IPlaylist
{
public:
   Playlist()
      :m_uiCurrentPos(0),
      m_uiNextPos(1)
   {
   }

   virtual ~Playlist() throw() {}

   size_t GetSize() const throw() { return m_vecPlaylistEntries.size(); }

   size_t GetCurrentPos() const throw() { return m_uiCurrentPos; }

   size_t GetNextPos() const throw() { return m_uiNextPos; }

   void SetCurrentPos(size_t uiPos) throw() { m_uiCurrentPos = uiPos; }
   void SetNextPos(size_t uiPos) throw() { m_uiNextPos = uiPos; }

   void Add(int iId, T_VideoId iVideoId, int iSortNr)
   {
      PlaylistEntry entry;
      entry.m_iVideoId = iVideoId;
      entry.m_iSortNr = iSortNr;

      entry.m_iId = iId;

      m_vecPlaylistEntries.push_back(entry);
   }

   void RemoveAt(size_t uiPos)
   {
      ATLASSERT(uiPos < m_vecPlaylistEntries.size());

      std::vector<PlaylistEntry>::iterator iterToDelete =
         m_vecPlaylistEntries.begin() + uiPos;

      // adjust current and next
      if (uiPos < m_uiCurrentPos)
         m_uiCurrentPos--;

      if (uiPos < m_uiNextPos)
         m_uiNextPos--;

      m_vecPlaylistEntries.erase(iterToDelete);
   }

   void Move(const std::vector<size_t>& vecEntryPosToMove, size_t uiInsertPos) throw();

   void MoveOne(size_t uiPosToMove, size_t uiInsertPos) throw()
   {
      std::vector<size_t> vecEntryPosToMove;
      vecEntryPosToMove.push_back(uiPosToMove);
      Move(vecEntryPosToMove, uiInsertPos);
   }

   // virtual functions from IPlaylist

   virtual bool IsNextAvail() const throw() override
   {
      return m_uiNextPos < m_vecPlaylistEntries.size();
   }

   virtual T_VideoId GetCurrentEntry() const throw() override
   {
      ATLASSERT(m_uiCurrentPos < m_vecPlaylistEntries.size());
      return m_vecPlaylistEntries[m_uiCurrentPos].m_iVideoId;
   }

   virtual T_VideoId GetNextEntry() const throw() override
   {
      ATLASSERT(IsNextAvail());
      return m_vecPlaylistEntries[m_uiNextPos].m_iVideoId;
   }

   virtual void NextToCurrent() throw() override
   {
      ATLASSERT(IsNextAvail());
      m_uiCurrentPos = m_uiNextPos;
      ++m_uiNextPos;
   }

   virtual void IncNextPos() throw() override
   {
      ATLASSERT(IsNextAvail());
      ++m_uiNextPos;
   }

   const PlaylistEntry& GetEntry(size_t uiPos) const throw()
   {
      ATLASSERT(uiPos < m_vecPlaylistEntries.size());
      return m_vecPlaylistEntries[uiPos];
   }

   PlaylistEntry& GetEntry(size_t uiPos) throw()
   {
      ATLASSERT(uiPos < m_vecPlaylistEntries.size());
      return m_vecPlaylistEntries[uiPos];
   }

   void Reset() throw() { SetCurrentPos(0); }

   bool IsEmpty() const throw() { return m_vecPlaylistEntries.empty(); }

   // TODO remove again
   std::vector<PlaylistEntry>& GetPlaylistEntries() { return m_vecPlaylistEntries; }

private:
   void MoveUp(const std::vector<size_t>& vecEntryPosToMove, size_t uiInsertPos) throw();

   void MoveDown(const std::vector<size_t>& vecEntryPosToMove, size_t uiInsertPos) throw();

protected: // TODO
   std::vector<PlaylistEntry> m_vecPlaylistEntries;
   size_t m_uiCurrentPos;
   size_t m_uiNextPos;
};
