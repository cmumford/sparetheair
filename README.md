# Spare the Air eInk Display [![Build Status](https://travis-ci.com/cmumford/sparetheair.svg?branch=master)](https://travis-ci.com/cmumford/sparetheair)

This is a project to display the current status, and forecast,
of the California San Francisco bay area's
[Spare the Air](http://www.sparetheair.org/) status. It is an Arduino project
which uses an ESP32 (which has built-in WiFi) to fetch the status and forecast
from the sparetheair.org RSS feed and display it on an
Adafruit [2.7" Tri-Color eInk / ePaper Display with SRAM](https://www.adafruit.com/product/4098).

## ESP32 info

1. Any ESP32 should work. Author uses the
   [HiLetgo ESP-WROOM-32 ESP32 ESP-32S Development Board](https://smile.amazon.com/dp/B0718T232Z/ref=cm_sw_em_r_mt_dp_U_NaB0DbY3RFH7X).
2. Install the esp32 board via the Arduino IDE's Boards Manager.
3. Install the appropriate [driver](https://www.silabs.com/products/development-tools/software/usb-to-uart-bridge-vcp-drivers)
   for your operating system.

## Building

This project uses the following Arduino libraries.

1. [HttpClient](https://www.arduino.cc/en/Tutorial/HttpClient)
2. [TinyXML](https://github.com/adafruit/TinyXML)
3. [ArduinoUnit](https://github.com/mmurdoch/arduinounit)
4. [Adafruit EPD Library](https://github.com/adafruit/Adafruit_EPD)
5. [Adafruit GFX Library](https://github.com/adafruit/Adafruit-GFX-Library)

To install follow [these instructions](https://www.arduino.cc/en/guide/libraries).

This project has two Arduino sketches:

1. sparetheair.ino (main app sketch).
2. lib/lib.ino (unit test sketch).
