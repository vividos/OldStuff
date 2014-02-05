# ExifShellExt #

## About ##

ExifShellExt is an Explorer shell extension that offers Explorer context menu entries to copy and paste Exif data from/to JPEG image files.

This comes in handy if you're editing a photo image and the software discards or modifies the original Exif data (e.g. Panorama or HDR processing software). With this shell extension you can copy over the Exif data from the original file.

## Download ##

You can download ExifShellExt here:
- 32-bit version: TODO (x kb)

Store the ExifShellExt.dll file somewhere and register the file using the following command:

`regsvr32 {path-to}\ExifShellExt.dll`

You need to install the Visual C++ 2012 Redistributable before using the shell extension:
[http://www.microsoft.com/en-us/download/details.aspx?id=30679](http://www.microsoft.com/en-us/download/details.aspx?id=30679)

## Technical Background ##

The shell extension works by parsing the JFIF (JPEG file interchange format) file structure and copying the APP1 block to the clipboard, using the clipboard format `CF_JPEG_EXIFDATA`. If it finds this clipboard format on the clipboard, the "Paste" menu item is available. Pasting Exif data rewrites the selected JPEG file (by parsing and writing the JFIF file structur), throws away the APP1 block and inserts the contents of the clipboard instead.
