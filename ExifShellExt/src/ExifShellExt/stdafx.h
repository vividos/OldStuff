//
// ExifShellExt - Exif data shell extension
// Copyright (C) 2008,2017,2020 Michael Fink
//
/// \file stdafx.h Precompiled header support
//
#pragma once

#include <SDKDDKVer.h>

#include <ulib/config/Wtl.hpp>

// Win32 includes
#include <shellapi.h>

// simulate atlmisc.h for atlctxmenu.h
#define __ATLMISC_H__

// shell extension includes
#include "atlctxmenu.h"
#include "atlshellex.h"
