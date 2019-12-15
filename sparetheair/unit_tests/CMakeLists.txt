cmake_minimum_required(VERSION 3.11)
project (unit_tests LANGUAGES CXX)

include(FetchContent)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(MOCK_ARDUINO_DIR "mock_arduino")
set(MYPROJECT_DIR ".")
set(SPARETHEAIR_DIR "..")
#get_filename_component(SPARETHEAIR_DIR ${MYPROJECT_DIR} DIRECTORY)

# Download and unpack googletest at configure time
configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

FetchContent_Declare(
  tinyxml
  GIT_REPOSITORY    https://github.com/adafruit/TinyXML.git
  GIT_TAG           master
  CONFIGURE_COMMAND ""
  BUILD_COMMAND     ""
  INSTALL_COMMAND   ""
  TEST_COMMAND      ""
)

FetchContent_GetProperties(tinyxml)
if(NOT tinyxml_POPULATED)
  FetchContent_Populate(tinyxml)
endif()

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
                 ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
                 EXCLUDE_FROM_ALL)

# The gtest/gtest_main targets carry header search path
# dependencies automatically when using CMake 2.8.11 or
# later. Otherwise we have to add them here ourselves.
if (CMAKE_VERSION VERSION_LESS 2.8.11)
  include_directories("${gtest_SOURCE_DIR}/include")
endif()

include_directories(
  "${MOCK_ARDUINO_DIR}"
  "${SPARETHEAIR_DIR}"
  "${tinyxml_SOURCE_DIR}"
  "."
)

add_library(Arduino "")
  target_sources(Arduino
  PRIVATE
    "${MOCK_ARDUINO_DIR}/Arduino_String.cpp"
    "${MOCK_ARDUINO_DIR}/Arduino_String.h"
    "${MOCK_ARDUINO_DIR}/Arduino.h"
    "${MOCK_ARDUINO_DIR}/Print.h"
    "${MOCK_ARDUINO_DIR}/SPI.h"
    "${MOCK_ARDUINO_DIR}/WProgram.h"
)

add_library(TinyXML "")
  target_sources(TinyXML
  PRIVATE
    "${tinyxml_SOURCE_DIR}/TinyXML.cpp"
)

add_library(sparetheair "")
  target_sources(sparetheair
  PRIVATE
    "${SPARETHEAIR_DIR}/network.cpp"
    "${SPARETHEAIR_DIR}/network.h"
    "${SPARETHEAIR_DIR}/parser.cpp"
    "${SPARETHEAIR_DIR}/parser.h"
    "${SPARETHEAIR_DIR}/status.cpp"
    "${SPARETHEAIR_DIR}/status.h"
)
target_compile_options(sparetheair
  PRIVATE -Wall -Wextra -pedantic -Werror)

add_executable(unit_tests main.cc)
target_link_libraries(unit_tests
  sparetheair
  TinyXML
  gtest_main
  Arduino
)
add_test(NAME unit_tests COMMAND unit_tests)
