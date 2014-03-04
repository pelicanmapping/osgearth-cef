
cmake_minimum_required (VERSION 2.8)
project (osgearthCef)

SET_PROPERTY( GLOBAL PROPERTY USE_FOLDERS               ON )
SET_PROPERTY( GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMake Targets" )

set (${PROJECT_NAME}_VERSION_MAJOR 1)
set (${PROJECT_NAME}_VERSION_MINOR 0)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules/;${CMAKE_MODULE_PATH}") 

# base setup 
SET(CMAKE_DEBUG_POSTFIX "d" CACHE STRING "add a postfix, usually d on windows")
SET(CMAKE_RELEASE_POSTFIX "" CACHE STRING "add a postfix, usually empty on windows")

# binary output
if(WIN32)
	if(CMAKE_CL_64)
		message(STATUS "64 bit compiler detected.")
		set(OUTPUT_BINDIR ${CMAKE_CURRENT_SOURCE_DIR}/bin/x64) 
	else()
		set(OUTPUT_BINDIR ${CMAKE_CURRENT_SOURCE_DIR}/bin/x86) 
		message(STATUS "WIN32 bin dir: ${OUTPUT_BINDIR}")
	endif()
endif()	

# Library output

if(WIN32)
  SET(OUTPUT_LIBDIR ${PROJECT_BINARY_DIR}/lib)
  MAKE_DIRECTORY(${OUTPUT_LIBDIR})

  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_BINDIR})
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_LIBDIR})
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_LIBDIR})

  FOREACH(CONF ${CMAKE_CONFIGURATION_TYPES})       
      STRING(TOUPPER "${CONF}" CONF)               
      STRING(TOLOWER "${CONF}" CONFL)
      SET("CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${CONF}" "${OUTPUT_LIBDIR}")
      SET("CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONF}" "${OUTPUT_BINDIR}/${CONFL}")
      IF(WIN32)
          SET("CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CONF}" "${OUTPUT_BINDIR}/${CONFL}")
      ELSE()
          SET("CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CONF}" "${OUTPUT_LIBDIR}")
      ENDIF()
  ENDFOREACH()

  LINK_DIRECTORIES( ${LINK_DIRECTORIES} ${OUTPUT_LIBDIR} )
endif()

# --- OSG / osgEarth

if(WIN32)

	# configure sdk's
	find_path(SDK_DIR NAMES create_lib.py PATHS c:/sdk NO_DEFAULT_PATH)	

	set(OSG_DIR ${SDK_DIR}/osg/3.2.0 ) 
	set(OSGEARTH_DIR ${SDK_DIR}/osgearth/2.5 )

	find_path(CEF_ROOT_DIR NAMES include/cef_version.h PATHS ${SDK_DIR}/cef/3.1547 NO_DEFAULT_PATH)	

	if(CMAKE_CL_64)
		set(LIBRARY_PATH_SUFFIXES win/64/release/lib win/64/debug/lib)
	else()
		set(LIBRARY_PATH_SUFFIXES win/32/release/lib win/32/debug/lib)
	endif()
	set(INCLUDE_PATH_SUFFIXES /inc/)
	set(CMAKE_CONFIGURATION_TYPES Debug Release)
	set(CMAKE_CONFIGURATION_TYPES "${CMAKE_CONFIGURATION_TYPES}" CACHE STRING
	 "Reset the configurations to what we need"
	 FORCE)
endif()


make_directory(${OUTPUT_BINDIR})

#--------------------------------------------------------------------------------
# source files 

set(APP_SRCS 
    BrowserClient.cpp
    CefHelper.cpp
    JsonArguments.cpp
    MapExecuteCallback.cpp
    RenderProcessHandler.cpp
)

#--------------------------------------------------------------------------------
# header files

set(APP_HDRS
    BrowserClient.h
    CefHelper.h
    ExecuteCallback.h
    JsonArguments.h
    MapExecuteCallback.h
    OECefApp.h
    RenderProcessHandler.h
)


find_package(OSG COMPONENTS osg osgViewer ) 
include_directories(${OSG_INCLUDE_DIRS} )
find_package(OSGEARTH REQUIRED)
include_directories(${OSGEARTH_INCLUDE_DIRS} )
find_package(CEF REQUIRED)
include_directories(${CEF_INCLUDE_DIR} )

message(STATUS "OSG found: ${OSG_FOUND} includes: ${OSG_INCLUDE_DIRS} libraries: ${OSG_LIBRARIES_DIR}")
message(STATUS "OSGEARTH found: ${OSGEARTH_FOUND} includes: ${OSGEARTH_INCLUDE_DIRS} libraries: ${OSGEARTH_LIBRARY} / ${OSGEARTH_LIBRARY_DEBUG}")

#--------------------------------------------------------------------------------

set(TARGET_SRC
  ${APP_SRCS}
  ${APP_HDRS}
)

# setup target

add_library(
	${PROJECT_NAME} 
	${TARGET_SRC} 
	${TARGET_H}
)

set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_OUTPUT_NAME "${PROJECT_NAME}${CMAKE_DEBUG_POSTFIX}")
set_target_properties(${PROJECT_NAME} PROPERTIES RELEASE_OUTPUT_NAME "${PROJECT_NAME}${CMAKE_RELEASE_POSTFIX}")


# Separate project for test app
add_executable( "osgearth_cef" applications/osgearth_cef/main.cpp )
add_executable( "osgearth_cef_simple" applications/osgearth_cef_simple/main.cpp )

set(EXT_LIBS
	OSGUTIL
	OSGDB
	OSGGA
	OSG
	OSGFX
	OSGSIM
	OSGMANIPULATOR
	OSGTERRAIN
	OSGTEXT
	OSGVIEWER
	OSGEARTH
	OSGEARTHANNOTATION
	OSGEARTHFEATURES
	OSGEARTHSYMBOLOGY
	OSGEARTHUTIL
	OPENTHREADS
)

foreach(ext_lib ${EXT_LIBS})
  STRING(TOUPPER "${ext_lib}" ext_lib_upper)

  target_link_libraries( "osgearth_cef" optimized "${${ext_lib_upper}_LIBRARY}" debug "${${ext_lib_upper}_LIBRARY_DEBUG}")
  target_link_libraries( "osgearth_cef_simple" optimized "${${ext_lib_upper}_LIBRARY}" debug "${${ext_lib_upper}_LIBRARY_DEBUG}")
	
endforeach()

set(CEF_LIBS
	CEF
	CEF_WRAPPER
)
# CEF libs uses _LIBRARY_RELEASE not _LIBRARY as a postfix
foreach(cef_lib ${CEF_LIBS})
  STRING(TOUPPER "${cef_lib}" cef_lib_upper)

  target_link_libraries( ${PROJECT_NAME} optimized "${${cef_lib_upper}_LIBRARY_RELEASE}" debug "${${cef_lib_upper}_LIBRARY_DEBUG}")
  target_link_libraries( "osgearth_cef_simple" optimized "${${cef_lib_upper}_LIBRARY_RELEASE}" debug "${${cef_lib_upper}_LIBRARY_DEBUG}")
	
endforeach()


target_link_libraries( "osgearth_cef" optimized "${PROJECT_NAME}" )
target_link_libraries( "osgearth_cef_simple" optimized "${PROJECT_NAME}" )

include_directories(${PROJECT_SOURCE_DIR} )


# this supports WIN32, MACOSX
#install(TARGETS ${PROJECT_NAME}
#    BUNDLE DESTINATION . COMPONENT Runtime
#    RUNTIME DESTINATION bin COMPONENT Runtime
#)



