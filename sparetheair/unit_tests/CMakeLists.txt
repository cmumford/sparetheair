cmake_minimum_required(VERSION 2.8.9)
project (unit_tests LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(TINYXML_DIR "../../../TinyXML")
set(MOCK_ARDUINO_DIR "mock_arduino")
set(MYPROJECT_DIR ".")
set(SPARETHEAIR_DIR "..")
#get_filename_component(SPARETHEAIR_DIR ${MYPROJECT_DIR} DIRECTORY)

include_directories(
  "${MOCK_ARDUINO_DIR}"
  "."
)

add_library(TinyXML "")
  target_sources(TinyXML
  PRIVATE
    "${TINYXML_DIR}/TinyXML.cpp"
)

target_include_directories(TinyXML
  PUBLIC
    $<BUILD_INTERFACE:${TINYXML_DIR}>
)

add_library(sta "")
  target_sources(sta
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

  #add_executable(unit_tests main.cc)
