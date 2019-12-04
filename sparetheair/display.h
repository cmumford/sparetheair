// Copyright 2019 Christopher Mumford
// This code is licensed under MIT license (see LICENSE for details)

#ifndef SPARETHEAIR_DISPLAY_H_
#define SPARETHEAIR_DISPLAY_H_

#include "Adafruit_EPD.h"
#include "network.h"

namespace spare_the_air {

struct Point;
struct Rectangle;

class Display {
 public:
  Display();
  ~Display();

  void Draw();

 private:
  void DrawString(const String& str, const Point& pt, uint16_t color);
  void DrawLogo();
  void DrawTodayEntry(const Status& status);
  void DrawForecastLines();
  void DrawForecast(const Status& status, const Rectangle& bounds);
  void DrawForecasts();
  void DrawAQIMeter(const Point& tl, AQICategory category);
  void DrawArrow(const Point& tip, uint8_t color);

  bool display_begun_;
  Adafruit_IL91874 display_;
};

}  // namespace spare_the_air

#endif  // SPARETHEAIR_DISPLAY_H_
