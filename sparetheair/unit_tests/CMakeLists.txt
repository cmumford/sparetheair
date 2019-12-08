cmake_minimum_required(VERSION 2.8.9)
project (unit_tests LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(TINYXML_DIR "../../../TinyXML")
set(ADAFRUIT_EPD_DIR "../../../Adafruit_EPD")
set(ADAFRUIT_GFX_DIR "../../../Adafruit-GFX-Library")
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
  "${ADAFRUIT_EPD_DIR}"
  "${ADAFRUIT_GFX_DIR}"
  "${MOCK_ARDUINO_DIR}"
  "${SPARETHEAIR_DIR}"
  "${TINYXML_DIR}"
  "."
)

add_library(Arduino "")
  target_sources(Arduino
  PRIVATE
    "${MOCK_ARDUINO_DIR}/String.cpp"
    "${MOCK_ARDUINO_DIR}/String.h"
    "${MOCK_ARDUINO_DIR}/Arduino.h"
    "${MOCK_ARDUINO_DIR}/Print.h"
    "${MOCK_ARDUINO_DIR}/SPI.h"
    "${MOCK_ARDUINO_DIR}/WProgram.h"
)

add_library(TinyXML "")
  target_sources(TinyXML
  PRIVATE
    "${TINYXML_DIR}/TinyXML.cpp"
)

add_library(HTTPClient "")
  target_sources(HTTPClient
  PRIVATE
    "${MOCK_ARDUINO_DIR}/HTTPClient.cpp"
    "${MOCK_ARDUINO_DIR}/HTTPClient.h"
)

add_library(Adafruit_EPD "")
  target_sources(Adafruit_EPD
  PRIVATE
    "${ADAFRUIT_EPD_DIR}/Adafruit_EPD.h"
    "${MOCK_ARDUINO_DIR}/Adafruit_EPD.cpp"
)

add_library(sparetheair "")
  target_sources(sparetheair
  PRIVATE
    "${SPARETHEAIR_DIR}/aqi_meter_image.h"
    "${SPARETHEAIR_DIR}/display.cpp"
    "${SPARETHEAIR_DIR}/display.h"
    "${SPARETHEAIR_DIR}/font_base.h"
    "${SPARETHEAIR_DIR}/font_large.h"
    "${SPARETHEAIR_DIR}/font_medium.h"
    "${SPARETHEAIR_DIR}/logo.h"
    "${SPARETHEAIR_DIR}/network.cpp"
    "${SPARETHEAIR_DIR}/network.h"
    "${SPARETHEAIR_DIR}/size.h"
)
target_compile_options(sparetheair
  PRIVATE -Wall -Wextra -pedantic -Werror)

add_executable(unit_tests main.cc)
target_link_libraries(unit_tests
  sparetheair
  Adafruit_EPD
  HTTPClient
  TinyXML
  gtest_main
  Arduino
)
add_test(NAME unit_tests COMMAND unit_tests)
