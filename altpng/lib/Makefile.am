#
# altpng - a versatile png reading and writing library
# Copyright (c) 2003,2004,2021 Michael Fink
#

# Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS = gnu

lib_LTLIBRARIES = libaltpng.la
include_HEADERS = altpng.h

EXTRA_DIST = altpng.h.pl

include Makefile.common

libaltpng_la_LDFLAGS = -version-info 0:0:0

noinst_HEADERS = $(altpng_headers)

BUILT_SOURCES = altpng.h
CLEANFILES = altpng.h

altpng.h: Makefile.am altpng.h.pl $(altpng_headers)
	perl -w altpng.h.pl $(altpng_headers)
