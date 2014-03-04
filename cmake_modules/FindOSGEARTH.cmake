
if(APPLE)
    SET(CMAKE_FIND_LIBRARY_SUFFIXES .dylib .so .a)
endif()

###### headers ######

macro( find_osgearth_include THIS_OSGEARTH_INCLUDE_DIR THIS_OSGEARTH_INCLUDE_FILE )

    FIND_PATH( ${THIS_OSGEARTH_INCLUDE_DIR} ${THIS_OSGEARTH_INCLUDE_FILE}
        PATHS
            ${OSGEARTH_DIR}
            /usr/local/
            [HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\Session\ Manager\\Environment;OSGEARTH_ROOT]/
            ~/Library/Frameworks
            /Library/Frameworks
        NO_DEFAULT_PATH
        PATH_SUFFIXES
            ${INCLUDE_PATH_SUFFIXES}
            osx/i64/include
    )

endmacro()

#find_osgearth_include( OSGEARTH_GEN_INCLUDE_DIR osgEarth/Common )
find_osgearth_include( OSGEARTH_INCLUDE_DIR     osgEarth/TileSource )

###### libraries ######

macro( find_osgearth_library MYLIBRARY MYLIBRARYNAME )

    FIND_LIBRARY(${MYLIBRARY}
        NAMES
            ${MYLIBRARYNAME}
        PATHS
            ${OSGEARTH_DIR}
            ${OSGEARTH_DIR}/../release
            ${OSGEARTH_DIR}/../debug
            ~/Library/Frameworks
            /Library/Frameworks
            /usr/local
            /opt/local
            [HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\Session\ Manager\\Environment;OSGEARTH_ROOT]/lib
        PATH_SUFFIXES
    		${LIBRARY_PATH_SUFFIXES}
    )

endmacro()

if(WIN32)
    find_osgearth_library( OSGEARTH_LIBRARY osgEarth )
    find_osgearth_library( OSGEARTH_LIBRARY_DEBUG osgEarthd)

    find_osgearth_library( OSGEARTHUTIL_LIBRARY osgEarthUtil )
    find_osgearth_library( OSGEARTHUTIL_LIBRARY_DEBUG osgEarthUtild)

    find_osgearth_library( OSGEARTHFEATURES_LIBRARY osgEarthFeatures )
    find_osgearth_library( OSGEARTHFEATURES_LIBRARY_DEBUG osgEarthFeaturesd)

    find_osgearth_library( OSGEARTHSYMBOLOGY_LIBRARY osgEarthSymbology )
    find_osgearth_library( OSGEARTHSYMBOLOGY_LIBRARY_DEBUG osgEarthSymbologyd )

    find_osgearth_library( OSGEARTHANNOTATION_LIBRARY osgEarthAnnotation )
    find_osgearth_library( OSGEARTHANNOTATION_LIBRARY_DEBUG osgEarthAnnotationd )
endif()


set( OSGEARTH_FOUND "NO" )
if(OSGEARTH_LIBRARY OR OSGEARTH_LIBRARY_DEBUG)
    if( OSGEARTH_INCLUDE_DIR )
        set( OSGEARTH_FOUND "YES" )
#        set( OSGEARTH_INCLUDE_DIRS ${OSGEARTH_INCLUDE_DIR} ${OSGEARTH_GEN_INCLUDE_DIR} )
        set( OSGEARTH_INCLUDE_DIRS ${OSGEARTH_INCLUDE_DIR} )
        get_filename_component( OSGEARTH_LIBRARIES_DIR ${OSGEARTH_LIBRARY} PATH )
    endif()
endif()
