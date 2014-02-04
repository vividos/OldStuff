# MrCrash #

## About ##

MrCrash is a tool to analyse applications that may crash (e.g. the application closes without any windows left, shows a "Windows Error Reporting" dialog or shows other crash-y behavior).

Error reporting information are collected then, and these infos can help developers to better determine the reason of the crash.

## Download ##

You can download MrCrash here:
TODO

Additionally you need to install the Visual C++ 2012 Redistributable:
[http://www.microsoft.com/en-us/download/details.aspx?id=30679](http://www.microsoft.com/en-us/download/details.aspx?id=30679)


## Technical Background ##

MrCrash acts as application debugger and records all debug events from the application. In case of second-chance exceptions (first-chance exceptions are normally handled by the application itself), app collected infos so far are written to an xml file. Additionally a minidump crash dump file (extension .mdmp) is created.

MrCrash needs admin privileges to debug applications, and when running under Windows Vista and above it therefore starts with an UAC prompt.