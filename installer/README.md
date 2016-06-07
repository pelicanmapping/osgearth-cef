# ReadyMap Packager Build Process

Instructions for building the ReadyMap Packager installer:

1. Create an empty build folder and copy the packager.iss (InnoSetup install script) file into
it.
2. Copy all osg, osgEarth, GDAL, etc. dependencies into the build folder.
NOTE:​Be sure to preserve osgPlugins subfolder structure to avoid conflicts with other
files on your local machine
3. Copy the CEF .pak files into the build folder.
4. Copy the osgearth_cef.exe executable into the build folder and rename it packager.exe
5. Copy everything from the osgearth­cef/applications/packager folder into the build folder,
preserving the folder structure.
NOTE:​the index.html file should be in the same location as the packager.exe
executable
6. Run InnoSetup and open the packager.iss file.
7. Change the OutputDir value to your desired location.
8. Click the Compile button.
For convenience the osgearth.js file is already in the osgearth­cef/applications/packager/js
folder. If any changes are made to the original osgearth.js file (osgearth­cef/js/osgearth.js), the
updated file should be copied ove