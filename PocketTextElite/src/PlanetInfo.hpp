//
// PocketTextElite - port of Elite[TM] trading system
// Copyright (C) 2008-2009 Michael Fink
//
/// \file PlanetInfo.hpp Planet information
//
#pragma once

/// planet information
class PlanetInfo
{
public:
   PlanetInfo(unsigned int uiPlanetIndex, const CString& cszName, unsigned int uiXPos, unsigned int uiYPos) throw()
      :m_uiPlanetIndex(uiPlanetIndex),
       m_cszName(cszName),
       m_uiXPos(uiXPos),
       m_uiYPos(uiYPos)
   {
   }

   unsigned int PlanetIndex() const throw() { return m_uiPlanetIndex; }

   CString Name() const throw() { return m_cszName; }

   unsigned int XPos() const throw() { return m_uiXPos; }
   unsigned int YPos() const throw() { return m_uiYPos; }

private:
   unsigned int m_uiPlanetIndex;
   CString m_cszName;
   unsigned int m_uiXPos;
   unsigned int m_uiYPos;
};
