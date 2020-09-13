# Makefile for the `shared` static library

CC= tcc

CFLAGS= -O2 -ml -w-par -w-stu -w-ccc -w-rch
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
LIBSOURCES= app.cpp cstring.cpp keyboard.cpp screen.cpp window.cpp

SOURCES= $(LIBSOURCES)

# files included by source files
INCLUDES= app.h cstring.h defs.h keyboard.h keycode.h linmap.h screen.h \
        window.h

# These objectfiles are included in shared.lib
LIBOBJECTS= app.obj cstring.obj keyboard.obj screen.obj window.obj

all: shared.lib

shared.lib: $(LIBOBJECTS)
	- del shared.lib
	tlib shared.lib /E /C @&&|
+app.obj +cstring.obj +keyboard.obj +screen.obj +window.obj , shared.lst
|

# This "{}" syntax allows Borland Make to "batch" source files.
# In this way, each run of the compiler can build many modules.
.cpp.obj:
	$(CC) $(CFLAGS) -c{ $<}

clean:
	- del *.obj
	- del shared.lib

app.obj: app.cpp app.h defs.h
cstring.obj: cstring.cpp cstring.h
keyboard.obj: keyboard.cpp keyboard.h keycode.h defs.h
screen.obj: screen.cpp screen.h defs.h window.h
window.obj: window.cpp window.h defs.h

