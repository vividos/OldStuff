#
# d64view - a d64 disk image viewer
# Copyright (c) 2002,2003,2016 Michael Fink
#
# automake/autoconf configuration file
#

# initial stuff
AC_PREREQ(2.53)
AC_INIT(source/app.cpp)

AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_CANONICAL_TARGET

# version info
PACKAGE=d64view
VERSION=0.99

AM_INIT_AUTOMAKE($PACKAGE,$VERSION,no-define)
AM_CONFIG_HEADER(source/config.h)

# initial variable values
CFLAGS="$CFLAGS"
CXXFLAGS="$CXXFLAGS"

# find and test the compiler
AC_PROG_CXX
AC_LANG_CPLUSPLUS

# other needed programs
AC_PROG_MAKE_SET
AC_PROG_INSTALL

# check standard C headers
AC_HEADER_STDC


# add compiler flags dependent on host system
case "$host_os" in
   *mingw32)
      CFLAGS="$CFLAGS -D__GNUWIN32__"
      CXXFLAGS="$CXXFLAGS -D__GNUWIN32__"
      ;;
esac


# check for wxWindows
AM_OPTIONS_WXCONFIG

AM_PATH_WXCONFIG(2.4.0, wxWin=1)
  if test "$wxWin" != 1; then
     AC_MSG_ERROR([
  	   wxWindows must be installed on your system
  	   but wx-config script couldn't be found.

  	   Please check that wx-config is in path, the directory
  	   where wxWindows libraries are installed (returned by
  	   'wx-config --libs' command) is in LD_LIBRARY_PATH or
  	   equivalent variable and wxWindows version is 2.3.4 or above.
     ])
  fi
  CPPFLAGS="$CPPFLAGS $WX_CPPFLAGS"
  CXXFLAGS="$CXXFLAGS $WX_CXXFLAGS_ONLY"
  CFLAGS="$CFLAGS $WX_CFLAGS_ONLY"

  # wxwin.m4 suggested putting $WX_LIBS to LDFLAGS, which clearly is the
  # wrong place. it belongs into LIBS, because it appears after the object
  # files in the linker command line
  LIBS="$LIBS $WX_LIBS"


# produce output
AC_CONFIG_FILES([
Makefile
source/Makefile
])

AC_OUTPUT


# output status
echo
echo " d64view is now ready to be built. Your current settings are:"
echo
echo "    host_os = $host_os"
echo "    prefix = ${prefix}"
echo "    datadir = ${datadir}"
echo
echo "    CXXFLAGS = $CXXFLAGS"
echo "    LIBS = $LIBS"
echo "    LDFLAGS = $LDFLAGS"
echo
