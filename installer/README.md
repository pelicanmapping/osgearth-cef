# ReadyMap Packager Build Process

The BUILD.bat script is used to copy over all the dependencies to a temporary build directory and package it into an installer.

1.  Install [InnoSetup](http://www.jrsoftware.org/isinfo.php) and make sure it's binaries are in your PATH
2.  If you need to override any of the environment variables, create a setenv.bat file in this directory and change any of the directory locations.
3.  Run BUILD.bat from this directory.  If all goes well it will copy over all of the dependencies to a "build" directory and generate an installer called ReadyMapPackager.exe
