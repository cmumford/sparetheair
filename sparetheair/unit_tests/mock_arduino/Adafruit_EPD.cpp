#include "Adafruit_EPD.h"

Adafruit_EPD::Adafruit_EPD(int width, int height,
    int8_t SID, int8_t SCLK, int8_t DC, int8_t RST,
    int8_t CS, int8_t SRCS, int8_t MISO, int8_t BUSY) :
Adafruit_GFX(width, height),
sram(SRCS) {}

Adafruit_EPD::Adafruit_EPD(int width, int height,
    int8_t DC, int8_t RST, int8_t CS, int8_t SRCS,
    int8_t BUSY) : Adafruit_GFX(width, height),
sram(SRCS) {}
