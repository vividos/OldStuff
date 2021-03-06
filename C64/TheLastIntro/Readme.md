# The Last Intro

## About

The Last Intro is the last (and incidentally, also the first) C64 intro coded
by me. The plan is to prepend the intro before some of my favourite games and
demos, just like the crackers in the old C64 times did, and then play/view
these games on the Raspberry Pi with RetroPie and the VICE C64 emulator.

## Intro contents

![Intro in WinVICE](the-last-intro.png)

Logo: A scrolling logo, moving in X direction (soft scroll) and Y direction
(FLD effect).

Music beat bar: Single raster line, visualizing the beat channel of the music
track playing.

Text scroll with raster bar: soft scroller with text.

The intro has a fade-in stage, a playing stage and a fade-out stage, in order
to not start and end the intro abruptly. The intro fades out when the user
presses the space bar or the joystick button of a joystick in port 2.

## Standalone

The standalone folder contains the standalone version of the intro. After
pressing the space bar or the joystick (in port 2) button, the intro resets
the C64 and returns to C64 basic.

# Development

In order to compile and run the intro, you need to download and extract two
packages:

- cc65 compiler: Place the current snapshot of the cc65 compiler into the
  folder tools/cc65, the ca65.exe should be in tools/cc65/bin/ca65.exe
- WinVICE emulator: place the latest version of winVice into the folder
  tools/winVICE, the c1541.exe should be in tools/winVICE/c1541.exe

Note that the "pucrunch" folder already contains the needed pucrunch.exe.

The Build.cmd script in every folder compiles and links the intro sourcecode
and the payload into a .d64 disk image file.

## Intro art

The intro logo was generated with a "Retro C64 logo creator" web page, and not
actually with a C64 drawing program. The resulting PNG image looks like this:

![Logo 320x200](logo_320_200.png)

The logo was converted to Koala format (file logo_320_200.kla) using
Timanthes_3_beta_v2, and then it was imported into Amica paint:

![Logo in Amica Paint](logo-amica-paint.png)

The logo was linearized, so that in each 8x8 char, the same color bit schemes
are used.

## Memory layout

This is the memory layout of the intro:

    $0801 - $0810 startup BASIC code - will get overwritten by intro
    $0800 - $0a00 charset
    $0a00 - $0b90 logo charset
    $0b90 - $0fff code
    $1000 - $1b00 music
    $1b00 - $1c7f logo data (scroll-x and scroll-y table, screen bytes)
    $1c80 - $1fff data, scroller text
    $2000 -       payload, e.g. game to start
