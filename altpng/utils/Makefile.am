#
# altpng - a versatile png reading and writing library
# Copyright (c) 2003,2004,2021 Michael Fink
#

# Process this file with automake to produce Makefile.in

INCLUDES = -I../lib

#
# altpng utils
#

noinst_PROGRAMS = pngmin altpngtest #pngview

pngmin_LDADD = ../lib/libaltpng.la

TESTS = altpngtest

altpngtest_SOURCES = altpngtest.c
altpngtest_LDADD = ../lib/libaltpng.la

EXTRA_DIST = pngview.cpp
