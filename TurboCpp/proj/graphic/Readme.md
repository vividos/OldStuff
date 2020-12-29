# Project graphic

This folder contains source files for graphics mode classes used in some
projects. The classe were refactored from original code to better fit modern
design patterns. Also direct linear framebuffer access was added.

You can compile the `graphic` static library by using the provided
`makefile`.

## Classes / Files

`graphic.h` Graphics interface.

`grdemo.cpp` Graphics demo application.

`modex.h/.cpp` Mode X class.

`vesamode.h/.cpp` VESA graphics mode class.

`vgamode.h/.cpp` VGA graphics mode class.

`xm_fkt.asm` Mode X assembler functions.
