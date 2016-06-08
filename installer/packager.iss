; -- packager.iss --
; Creates an installer for the ReadyMap Packager.
;
; Looks for files in the current directory and creates an installer that installs to the 64-bit Program Files directory.

[Setup]
AppName=ReadyMap Packager
AppVersion=0.5
DefaultDirName={pf64}\ReadyMap Packager
DefaultGroupName=ReadyMap Packager
UninstallDisplayIcon={app}\packager.exe
Compression=lzma2
SolidCompression=no
OutputDir=.
OutputBaseFilename=ReadyMapPackager

[Files]
Source: "build\*"; DestDir: "{app}"; Flags: recursesubdirs

[Icons]
Name: "{group}\ReadyMap Packager"; Filename: "{app}\packager.exe"; IconFilename: "{app}\img\favicon.ico"
