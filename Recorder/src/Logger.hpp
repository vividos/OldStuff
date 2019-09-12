//
// Recorder - a GPS logger app for Windows Mobile
// Copyright (C) 2006-2019 Michael Fink
//
/// \file Logger.hpp Logger functions
//
#pragma once

/// logs messages to debug console
void LogDebug(LPCTSTR msg, LPCTSTR category);

/// logs info messages to log file
void LogInfo(LPCTSTR msg, LPCTSTR category);

#define LOG_DEBUG(msg, category) LogDebug(##msg, ##category);
#define LOG_INFO(msg, category) LogInfo(##msg, ##category);
