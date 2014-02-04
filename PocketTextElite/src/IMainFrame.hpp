//
// PocketTextElite - port of Elite[TM] trading system
// Copyright (C) 2008-2009 Michael Fink
//
/// \file IMainFrame.hpp Main frame interface
//
#pragma once

/// main frame interface
class IMainFrame
{
public:
   virtual ~IMainFrame() throw()
   {
   }

   /// shows response from user action
   virtual void ShowResponse(LPCTSTR pszText, bool bSuccess) = 0;

   /// changes currently selected planet
   virtual void SelectPlanet(unsigned int uiSelectedPlanet) = 0;

   /// new planet system was entered
   virtual void EnteredNewSystem() = 0;
};
