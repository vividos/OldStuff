#
# altpng - a versatile png reading and writing library
# Copyright (c) 2003,2004,2021 Michael Fink
#
AC_REVISION([$Id: configure.ac,v 1.3 2003/03/01 13:50:42 vividos Exp $])

# Process this file with autoconf to produce a configure script.

# Initial stuff.
AC_PREREQ(2.53)
AC_INIT(lib/decoder.c)

# Version info
PACKAGE=altpng
VERSION=0.0.4

AM_INIT_AUTOMAKE($PACKAGE,$VERSION)

AM_CONFIG_HEADER(config.h)

dnl System type.

AC_CANONICAL_HOST

dnl Check for programs.

AC_LANG_C
AC_PROG_CC
AC_PROG_MAKE_SET
AC_PROG_INSTALL

dnl Support for libtool

AM_PROG_LIBTOOL

dnl Checks for header files.

AC_HEADER_STDC

dnl Checks for compiler characteristics

AC_CHECK_SIZEOF(unsigned char, 1)
AC_CHECK_SIZEOF(unsigned short, 2)
AC_CHECK_SIZEOF(int, 2)
AC_CHECK_SIZEOF(long, 4)

AC_C_BIGENDIAN
AC_TYPE_SIZE_T

dnl Checks for zlib.

AC_CHECK_HEADERS(zlib.h,,AC_MSG_ERROR([
required header file zlib.h missing]))

AC_SEARCH_LIBS(inflate,z)

AC_DEFINE(PACKAGE)
AC_DEFINE(VERSION)

dnl produce Makefiles
AC_OUTPUT([
   Makefile lib/Makefile utils/Makefile
])
