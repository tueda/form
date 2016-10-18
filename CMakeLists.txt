cmake_minimum_required(VERSION 2.6)
project(form)

include(CheckCSourceCompiles)
include(CheckIncludeFileCXX)
include(CheckTypeSize)

# Extract the FORM version.

file(STRINGS "${PROJECT_SOURCE_DIR}/sources/form3.h" FORM_MAJOR_VERSION
  REGEX "MAJORVERSION")
string(REGEX REPLACE "#define  *MAJORVERSION *" "" FORM_MAJOR_VERSION
  "${FORM_MAJOR_VERSION}")

file(STRINGS "${PROJECT_SOURCE_DIR}/sources/form3.h" FORM_MINOR_VERSION
  REGEX "MINORVERSION")
string(REGEX REPLACE "#define  *MINORVERSION *" "" FORM_MINOR_VERSION
  "${FORM_MINOR_VERSION}")

# Check for OS.

if(UNIX)
  # Includes OS X and Cygwin.
elseif(WIN32)
  set(WINDOWS TRUE)
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  set(LINUX TRUE)
endif()

if(UNIX AND LINUX)
  set(os_name "Linux")
elseif(UNIX)
  set(os_name "Unix")
elseif(WINDOWS)
  set(os_name "Windows")
else()
  message(FATAL_ERROR "${CMAKE_SYSTEM_NAME} not supported")
endif()

# Check for date model.

check_type_size(char SIZEOF_CHAR)
check_type_size(short SIZEOF_SHORT)
check_type_size(int SIZEOF_INT)
check_type_size(long SIZEOF_LONG)
check_type_size("long long" SIZEOF_LONG_LONG)
check_type_size(void* SIZEOF_VOID_P)

set(sizeof_types "${SIZEOF_CHAR}-${SIZEOF_SHORT}-${SIZEOF_INT}-${SIZEOF_LONG}-${SIZEOF_LONG_LONG}-${SIZEOF_VOID_P}")

if(sizeof_types MATCHES "1-2-4-4-8-4")
  set(ILP32 TRUE)
  set(data_model "ILP32")
elseif(sizeof_types MATCHES "1-2-4-4-8-8")
  set(LLP64 TRUE)
  set(data_model "LLP64")
elseif(sizeof_types MATCHES "1-2-4-8-8-8")
  set(LP64 TRUE)
  set(data_model "LP64")
else()
  message(FATAL_ERROR "Data model ${sizeof_types} not supported")
endif()

# Check for "inline" keyword in C.

function(check_c_inline)
  unset(inline PARENT_SCOPE)
  foreach(inline_keyword "inline" "__inline__" "__inline")
    string(TOUPPER "HAVE_C_${inline_keyword}" value)
    check_c_source_compiles("
      typedef int foo_t;
      static ${inline_keyword} foo_t static_foo() { return 0; }
      ${inline_keyword} foo_t foo() { return 0; }
      int main() { return 0; }
    " ${value})
    if(${value})
      if("${inline_keyword}" STREQUAL "inline")
      else()
        set(inline "${inline_keyword}" PARENT_SCOPE)
      endif()
      return()
    endif()
  endforeach()
  set(inline " " PARENT_SCOPE)
endfunction()

check_c_inline()

# Check for header files.

check_include_file_cxx(boost/unordered_map.hpp HAVE_BOOST_UNORDERED_MAP_HPP)
check_include_file_cxx(boost/unordered_set.hpp HAVE_BOOST_UNORDERED_SET_HPP)
check_include_file_cxx(tr1/unordered_map HAVE_TR1_UNORDERED_MAP)
check_include_file_cxx(tr1/unordered_set HAVE_TR1_UNORDERED_SET)
check_include_file_cxx(unordered_map HAVE_UNORDERED_MAP)
check_include_file_cxx(unordered_set HAVE_UNORDERED_SET)

# Process subdirectories.

add_subdirectory(sources)
