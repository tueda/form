# Generate config.h.

configure_file(
  "${PROJECT_SOURCE_DIR}/sources/config-cmake.h.in"
  "${PROJECT_BINARY_DIR}/config.h"
)
include_directories("${PROJECT_BINARY_DIR}")

add_definitions(-DHAVE_CONFIG_H)

# Generate version.h.

configure_file(
  "${PROJECT_SOURCE_DIR}/sources/version-cmake.h.in"
  "${PROJECT_BINARY_DIR}/sources/version.h"
)
include_directories("${PROJECT_BINARY_DIR}/sources")

# Debugging flag.

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DDEBUGGING")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUGGING")

# Executables.

function(read_automake_source begin_tag output_variable)
  # Read source files from the Makefile.am.
  file(STRINGS "${PROJECT_SOURCE_DIR}/sources/Makefile.am" text)
  set(files)
  foreach(line ${text})
    if("${line}" MATCHES "${begin_tag}")
      set(files ${line})
      break()
    endif()
  endforeach()
  string(REGEX REPLACE ".*=" "" files "${files}")
  string(REGEX REPLACE "[;\t\n\\]" " " files "${files}")
  string(REGEX REPLACE "[^ ]*\\.h" "" files "${files}")
  string(REGEX REPLACE "  +" " " files "${files}")
  string(STRIP "${files}" files)
  string(REGEX REPLACE " " ";" files "${files}")
  set("${output_variable}" "${files}" PARENT_SCOPE)
endfunction()

read_automake_source("SRCBASE" SRCBASE)
read_automake_source("SRCPTHREAD" SRCPTHREAD)
read_automake_source("SRCPARALLEL" SRCPARALLEL)

if(UNIX)
  set(SRCBASE ${SRCBASE} unixfile.c)
endif()

set(form_SOURCES ${SRCBASE})
set(tform_SOURCES ${SRCBASE} ${SRCPTHREAD})
set(parform_SOURCES ${SRCBASE} ${SRCPARALLEL})

add_executable(form ${form_SOURCES})
