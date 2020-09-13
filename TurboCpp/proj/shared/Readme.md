# Project shared

This folder contains several source files for classes that are used in several
projects. Not all files were in this folder originally, and some were modified
for style, and formatting and compiling. Some of the classes (like app, screen
and textwindow) were reconstructed since they were lost in time.

You can compile the `shared` static library by using the provided
`makefile`.

## Classes / Files

`defs.h` Common defines.

`app` Base class for applications, including a main (rendering) loop and
command line parsing.

`screen` Text mode screen, including text output, setting cursor and opening
`textwindow` instances.

`textwindow` Text (sub-)window that defines a rectangular area where output
is restricted while the `textwindow` instance is active.

`string` A fixed buffer string, including conversion from/to `const char*`
operators, conversion to integer and a `format()` method `printf`-style.

`keyboard` Keyboard access methods in a static class.

`linear_mapping` Mapping between integer range and float range.

`shrdtest.cpp` - Unit test for some of the above classes.
