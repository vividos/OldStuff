# Project xmode

This folder contains the source files for the pre-compiled `xmode.lib`. It
provides a class `xmode` that uses the famous VGA Mode X to display 320x200
paletted graphics. See here:
https://en.wikipedia.org/wiki/Mode_X
http://twimgs.com/ddj/abrashblackbook/gpbb47.pdf

The assembler source file came from a germal book called "PC Intern 4.0.
Systemprogrammierung" from Michael Tischer (ISBN : 978-3815810941).

In order to compile the library, Turbo C++ 3.0 is needed, since this is the
first version that can compile static libraries in the IDE. Also TASM is
needed, which isn't bundled into Turbo C++ installers.
