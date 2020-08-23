# Turbo C++ projects

This folder contains several ancient projects written using the Turbo C++ IDE.
All projects target 16-bit MS-DOS.

Note that this folder can be mounted in Dosbox as the C: drive and the
absolute paths used in the projects match perfectly.

## Turbo C++ 1.01 setup

In order to install and run Turbo C++ you need Doxbox, which you can get here:
https://www.dosbox.com/
(you can also use any old 386 PC you can find lying around)

You also need to download Turbo C++ 1.01 from the Borlard Developer Network
Museum, conserved by the Archive:
https://web.archive.org/web/20050204085204/http://bdn.borland.com/museum

Unzip the tcpp101.zip into this folder. Then start Dosbox and mount this
folder as the C: drive, e.g. with this command:

    "C:\...\DOSBOX\dosbox.exe" -c "mount c ." -c "C:"

You can also use the `run_tcpp.cmd` file provided here. You probably have to
adjust the dosbox path.

Change into the `tcpp101` folder (`cd tcpp101`) and start the install
program.

![Install program start](install-tcpp101-01.png)

Press enter to continue. The following screen appears:

![Install program 2](install-tcpp101-02.png)

Leave every option as is, as `C:\TC` is the expected path for Turbo C++.
Select `Start Installation` and wait until unpacking has finished.

As last step you can add the `C:\TC\BIN` path to the `PATH` environment
variable. The provided .cmd script also does that for you.

Finally, start Turbo C++ using `tc`.
