# SDL2 #

This folder contains the SDL2 library, header files and library files for
linking on x86 and x64 platforms.

## Android ##

Additionally it contains the Solution and Project files for Microsoft Visual
Studio 2015 to compile the library using the Android C++ project type. For an
example how to use the compiled library, see the "Intro.Android" and
"Emulator.Android" projects some subfolders above this folder.

To actually compile SDL2 for Android, either extract SDL2 (I used version
2.0.4) into this folder, so that it matches the "include" and "VisualC"
folders and open the "VisualC\SDL2.Android.sln" solution. Or you can copy the
contents of the "VisualC" folder into your own copy of SDL2.

You need both the libSDL2.so and the libSDLmain.a file to link to SDL. The
libSDL2.so should be added to your "Packaging" project (the one that actually
creates the .apk file). The libSDLmain.a project must also be added to the
linker command line of the "Shared object" project that acutally implements
your Android app. Again, see the "Intro.Android" and "Intro.Android.Packaging"
projects for details how to configure this.

To actually get SDL2 with your Android app to work, you need to do some more
things. SDL2 has a good instruction readme, check out the "android-project"
subfolder and the "docs\README-android.md" of SDL2, and look into my projects.
