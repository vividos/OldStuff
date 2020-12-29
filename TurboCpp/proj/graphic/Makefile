# Makefile for the `graphic` static library

CC= tcc

CFLAGS= -O2 -ml -w-par -w-stu -w-ccc -w-rch -I..\shared
# -O2 enables full code optimization (for pre-3.0 Borland C++, use -O -G -Z).
# -mm selects medium memory model (near data, far code pointers; DOS only!)
# -w-par suppresses warnings about unused function parameters
# -w-stu suppresses warnings about incomplete structures
# -w-ccc suppresses warnings about compile-time-constant conditions
# -w-rch suppresses warnings about unreachable code

LDFLAGS= -ml
# memory model option here must match CFLAGS!

# End of configurable options.

# source files
LIBSOURCES= graphic.cpp vgamode.cpp vesamode.cpp modex.cpp

SOURCES= $(LIBSOURCES)

# files included by source files
INCLUDES= graphic.h vgamode.h vesamode.h modex.h

# These objectfiles are included in graphic.lib
LIBOBJECTS= graphic.obj vgamode.obj vesamode.obj modex.obj

all: graphic.lib

graphic.lib: $(LIBOBJECTS)
	- del graphic.lib
	tlib graphic.lib /E /C @&&|
+graphic.obj +vgamode.obj +vesamode.obj +modex.obj , graphic.lst
|

# This "{}" syntax allows Borland Make to "batch" source files.
# In this way, each run of the compiler can build many modules.
.cpp.obj:
	$(CC) $(CFLAGS) -c{ $<}

clean:
	- del *.obj
	- del graphic.lib

graphic.obj: graphic.cpp graphic.h
vgamode.obj: vgamode.cpp vgamode.h
vesamode.obj: vesamode.cpp vesamode.h
modex.obj: modex.cpp modex.h
