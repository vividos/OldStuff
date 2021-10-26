# Makefile for the `misc` projects

CC= tcc

CFLAGS= -O2 -ml -w-par -w-stu -w-ccc -w-rch

LIBPATH = C:\PROJ\SHARED

all: netbeat.exe setilog.exe

# misc projects

netbeat.exe: netbeat.cpp
setilog.exe: setilog.cpp

# general rules

.cpp.exe:
  $(CC) $(CFLAGS) -L$(LIBPATH) $<

clean:
	- del *.obj
	- del *.exe
