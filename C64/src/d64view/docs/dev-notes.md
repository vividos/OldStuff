# developer notes #

This document contains various infos about development of d64view in no
specific order.

d64view is written using wxWindows. Get the latest package at
http://www.wxwindows.org/ and compile the library. Set the environment
variable WXWIN to let the compiler know where the wxWindows includes can be
found.

Compiling d64view under mingw32 using MSYS:
Make sure the variable WXWIN is set properly in Makefile.mingw (to the root
dir of the wxMSW installation) and type:
   make -f Makefile.mingw <target>
Targets supported are "all", "d64view", "clean" and "strip" (which all do the
obvious).

Compiling d64view under mingw32, MSYS and MSYS-dtk:
Make sure that the wxWindows libraries were compiled and installed properly
and you have a working wx-config script. In MSYS, just type
   ./autogen.sh
   ./configure
   make

You can use the --with-wx-prefix= parameter to specify an alternative
wxWindows path.

Possible make targets are (Linux and MSYS-dtk): "all", "clean", "distclean",
"dist", "install" and "uninstall".   

To do a new release, adjust version numbers in the following files:
configure.ac
source/version.hpp
win32/d64view.nsi
docs/doxygen.cfg

Source code documentation can be generated via Doxygen (see
http://www.doxygen.org/), using the command line (in subdir "docs"):
   doxygen doxygen.cfg

TODO-List:
- support for 36..40 block disk image bam's
  blocks are read in, but bam bits aren't, because there might be differences
  between Dolphin DOS and Speed DOS formatted disks (see d64.txt for more).

- show geos date info/file type/icon?
  geos file date, type and possibly icon could be shown in the dir listing.
  canvas and view have to be resized for this and infos have to be added to
  the "dir_entry" struct.

- properly support printing dir listings
  when printing, only use black/white colors

- properly implement about box
  about box has to be implemented nicely, with ok buttons and such.

- do autoconf support for linux (in progress)
  add autoconf/automake support for linux or mingw/msys/msys-dtk so linux
  people can use the tool, too.
