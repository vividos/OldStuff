//
// ExifShellExt - Exif data shell extension
// Copyright (C) 2008,2017 Michael Fink
//
/// \file stdafx.h Precompiled header support
//
#pragma once

#include <SDKDDKVer.h>

// temporary fix: don't include atlribbon.h from WTL 9.1, the code is not compatible with Visual Studio 2017
#define __ATLRIBBON_H__

#include <ulib/config/Wtl.hpp>

// Win32 includes
#include <shellapi.h>

// simulate atlmisc.h for atlctxmenu.h
#define __ATLMISC_H__

// shell extension includes
#include "atlctxmenu.h"
#include "atlshellex.h"
