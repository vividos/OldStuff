//
// PartyVideoDJ - YouTube crossfading app
// Copyright (C) 2008-2017 Michael Fink
//
/// \file IconList.hpp Icon list defines
//
#pragma once

/// icons in IDB_ICONLIST
enum T_enIcons
{
   iconFilm = 0,
   iconArrow = 1,
   iconNext = 2,
   iconError = 3,
};

inline void LoadIconList(CImageList& ilIcons, UINT uiBitmapId = IDB_ICONLIST)
{
   // load image list
   ilIcons.Create(16, 15, ILC_MASK | ILC_COLOR32, 0, 0);
   CBitmap bmpIcons;
   bmpIcons.LoadBitmap(uiBitmapId);
   ilIcons.Add(bmpIcons, RGB(253, 5, 255)); // pink mask
}
