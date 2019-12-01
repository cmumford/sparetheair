// Copyright 2019 Christopher Mumford
// This code is licensed under MIT license (see LICENSE for details)

#include "display.h"
#include "font_base.h"
#include "font_large.h"
#include "logo.h"
#include "network.h"
#include "size.h"

namespace sta {

struct Point {
  int x;
  int y;
};

struct Rectangle {
  Point tl;
  Point br;

  int left() const { return tl.x; }
  int top() const { return tl.y; }
  int right() const { return br.x; }
  int bottom() const { return br.y; }

  int width() const { return br.x - tl.x; }
  int height() const { return br.y - tl.y; }
};

namespace {

#define EPD_CS 25     // EInk Chip Select.
#define EPD_DC 26     // EInk Data/Command.
#define SRAM_CS -1    // SRAM Chip Select.
#define EPD_RESET 12  // Share with microcontroller Reset!
#define EPD_BUSY -1   // Not use a pin (unused on 2.7" display).

const Size kEPaperSize = {264, 176};

const Rectangle kEPaperBounds = {
    Point({0, 0}), Point({kEPaperSize.width - 1, kEPaperSize.height - 1})};
const Rectangle kTodayBounds = {Point({0, 0}),
                                Point({kEPaperBounds.right(), 88})};
const int kForecastWidth = kEPaperSize.width / 3;

// There are 3 forecast sections so 2 dividers between them.
const int kDividers[kNumStatusDays - 1] = {
    kEPaperSize.width * 1 / 3,
    kEPaperSize.width * 2 / 3,
};

const Rectangle kForecastBounds[kNumStatusDays - 1] = {
    {Point({0 * kForecastWidth, kTodayBounds.bottom()}),
     Point({kDividers[0] - 1, kEPaperBounds.bottom()})},
    {Point({kDividers[0], kTodayBounds.bottom()}),
     Point({kDividers[1] - 1, kEPaperBounds.bottom()})},
    {Point({kDividers[1], kTodayBounds.bottom()}),
     Point({kEPaperSize.width - 1, kEPaperBounds.bottom()})},
};

const Size kAQIMeterSize = {10, 6 * 10};

const uint16_t kWhiteColor = EPD_WHITE;
const uint16_t kBlackColor = EPD_BLACK;
const uint16_t kRedColor = EPD_RED;

// Just for debugging.
const bool kDrawBorders = false;

const int kNormalFontHeight = 14;
const int kLargeFontHeight = 36;

String GetDayOfWeekAbbrev(const String& dow) {
  return dow.substring(0, 3);
}

uint8_t GetPixelColor(int pixel) {
  if (pixel == 2)
    return kBlackColor;
  if (pixel == 1)
    return kRedColor;
  return kWhiteColor;
}

int CatToInt(AQICategory cat) {
  return static_cast<int>(cat);
}

}  // namespace

Display::Display()
    : display_begun_(false),
      display_(kEPaperSize.width,
               kEPaperSize.height,
               EPD_DC,
               EPD_RESET,
               EPD_CS,
               SRAM_CS) {}

Display::~Display() = default;

void Display::Draw() {
  if (!display_begun_) {
    display_.begin();
    display_begun_ = true;
  }
  display_.clearBuffer();

  DrawLogo();
  DrawTodayEntry(Network::status(0));
  DrawForecastLines();
  DrawForecasts();

  display_.display();
}

void Display::DrawLogo() {
  const int kMargin = 4;
  const int left = kEPaperSize.width - kLogoSize.width - kMargin;
  for (int x = 0; x < kLogoSize.width; x++) {
    for (int y = 0; y < kLogoSize.height; y++) {
      int pixel = kLogo[y * kLogoSize.width + x];
      display_.writePixel(left + x, kMargin + y, GetPixelColor(pixel));
    }
  }
}

void Display::DrawString(const String& str, const Point& pt, uint16_t color) {
  display_.setCursor(pt.x, pt.y);
  display_.setTextColor(color);
  display_.setTextWrap(false);
  display_.print(str);
}

void Display::DrawTodayEntry(const Status& status) {
  display_.setFont(&windows_command_prompt16pt7b);
  if (kDrawBorders) {
    display_.drawRect(kTodayBounds.left(), kTodayBounds.top(),
                      kTodayBounds.width(), kTodayBounds.height(), kRedColor);
  }
  const int kMargin = 4;
  DrawString(status.date_full, Point({kMargin, kMargin + kNormalFontHeight}),
             kBlackColor);

  DrawString(status.alert_status, Point({kMargin + 8, 50 + kNormalFontHeight}),
             status.AlertInEffect() ? kRedColor : kBlackColor);

  DrawAQIMeter({kTodayBounds.right() - 2 * kMargin - kAQIMeterSize.width -
                    kLogoSize.width,
                20},
               status.aqi_category);
}

void Display::DrawForecastLines() {
  display_.drawLine(kTodayBounds.left(), kTodayBounds.bottom(),
                    kTodayBounds.right(), kTodayBounds.bottom(), kBlackColor);
  for (int i = 0; i < kNumStatusDays - 1; i++) {
    const Rectangle fr = kForecastBounds[i];
    display_.drawLine(fr.right(), fr.top(), fr.right(), fr.bottom(),
                      kBlackColor);
  }
}

void Display::DrawForecast(const Status& status, const Rectangle& bounds) {
  if (kDrawBorders) {
    display_.drawRect(bounds.left(), bounds.top(), bounds.width(),
                      bounds.height(), kRedColor);
  }
  display_.setFont(&windows_command_prompt16pt7b);
  const int kMargin = 4;
  DrawString(GetDayOfWeekAbbrev(status.day_of_week),
             Point({bounds.left() + kMargin,
                    bounds.top() + kMargin + kNormalFontHeight}),
             kBlackColor);

  display_.setFont(&LibreBaskerville_Bold28pt7b);
  const int kLineHeight = kNormalFontHeight;
  String aqi_str = status.aqi_val != -1
                       ? String(status.aqi_val)
                       : Network::AQICategoryAbbrev(status.aqi_category);

  DrawString(
      aqi_str,
      Point({bounds.left() + kMargin, bounds.top() + kMargin + kLineHeight +
                                          kMargin + kLargeFontHeight}),
      kBlackColor);

  DrawAQIMeter({bounds.right() - kMargin - kAQIMeterSize.width,
                bounds.bottom() - kMargin - kAQIMeterSize.height},
               status.aqi_category);
}

void Display::DrawForecasts() {
  // Start at idx=1 because 0 is "today".
  for (int i = 1; i < kNumStatusDays; i++)
    DrawForecast(Network::status(i), kForecastBounds[i - 1]);
}

void Display::DrawArrow(const Point& tip, uint8_t color) {
  const int kWidth = 4;
  for (int i = 0; i < kWidth; i++) {
    for (int y = tip.y - i; y <= tip.y + i; y++) {
      display_.writePixel(tip.x - i, y, color);
    }
  }
}

void Display::DrawAQIMeter(const Point& tl, AQICategory category) {
  const int kMeterBlockHeight = kAQIMeterSize.width;
  display_.drawRect(tl.x, tl.y, kAQIMeterSize.width, kAQIMeterSize.height,
                    kBlackColor);
  const int kNumCategories = 6;
  int y = tl.y + kMeterBlockHeight;
  for (int i = 0; i < kNumCategories - 1; i++, y += kMeterBlockHeight)
    display_.drawLine(tl.x, y, tl.x + kAQIMeterSize.width, y, kBlackColor);

  if (category == AQICategory::None)
    return;

  // Now draw the little arrow.
  int int_val = CatToInt(category);
  y = tl.y + kAQIMeterSize.height - int_val * kMeterBlockHeight -
      kMeterBlockHeight / 2;
  DrawArrow({tl.x - 2, y}, kBlackColor);
}

}  // namespace sta
