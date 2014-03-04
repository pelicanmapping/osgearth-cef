# Borrowed from: Steven Lamerton
# Find module for the Chromium Embedded Framework

include(SelectLibraryConfigurations)
include(FindPackageHandleStandardArgs)

set(CEF_ROOT_DIR "" CACHE PATH "CEF root directory")

find_path(CEF_INCLUDE_DIR "include/cef_version.h"
          HINTS ${CEF_ROOT_DIR})

# Find the dll_wrapper
find_library(CEF_WRAPPER_LIBRARY_RELEASE NAMES libcef_dll_wrapper
             HINTS "${CEF_ROOT_DIR}"
             PATH_SUFFIXES "win/32/release/lib" ${LIBRARY_PATH_SUFFIXES}
			)


find_library(CEF_WRAPPER_LIBRARY_DEBUG NAMES libcef_dll_wrapper
             HINTS "${CEF_ROOT_DIR}"
             PATH_SUFFIXES "win/32/debug/lib" ${LIBRARY_PATH_SUFFIXES} 
			)
select_library_configurations(CEF_WRAPPER)

# Find the cef_sandbox

find_library(CEF_SANDBOX_LIBRARY_RELEASE NAMES cef_sandbox
             HINTS "${CEF_ROOT_DIR}"
             PATH_SUFFIXES "win/32/release/lib" ${LIBRARY_PATH_SUFFIXES}
            )

find_library(CEF_SANDBOX_LIBRARY_DEBUG NAMES cef_sandbox
             HINTS "${CEF_ROOT_DIR}"
             PATH_SUFFIXES "win/32/debug/lib" ${LIBRARY_PATH_SUFFIXES}
            )
select_library_configurations(CEF_SANDBOX)

# Find the library itself
find_library(CEF_LIBRARY_RELEASE NAMES libcef
             HINTS "${CEF_ROOT_DIR}"
             PATH_SUFFIXES "win/32/release/lib" ${LIBRARY_PATH_SUFFIXES}
			)
find_library(CEF_LIBRARY_DEBUG NAMES libcef
             HINTS "${CEF_ROOT_DIR}"
             PATH_SUFFIXES "win/32/debug/lib" ${LIBRARY_PATH_SUFFIXES} 
			)
select_library_configurations(CEF)

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(CEF DEFAULT_MSG
                                  CEF_INCLUDE_DIR)

mark_as_advanced(CEF_INCLUDE_DIR)