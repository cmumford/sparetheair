// Copyright 2019 Christopher Mumford
// This code is licensed under MIT license (see LICENSE for details)

#include "display.h"
#include "font_base.h"
#include "font_large.h"
#include "logo.h"
#include "network.h"
#include "size.h"

namespace spare_the_air {

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

constexpr const Size kEPaperSize = {264, 176};

constexpr const Rectangle kEPaperBounds = {
    Point({0, 0}), Point({kEPaperSize.width - 1, kEPaperSize.height - 1})};
const Rectangle kTodayBounds = {Point({0, 0}),
                                Point({kEPaperBounds.right(), 88})};
constexpr const int kForecastWidth = kEPaperSize.width / 3;

// There are 3 forecast sections so 2 dividers between them.
constexpr const int kDividers[kNumStatusDays - 1] = {
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

constexpr const Size kAQIMeterSize = {10, 6 * 10};

constexpr const uint16_t kWhiteColor = EPD_WHITE;
constexpr const uint16_t kBlackColor = EPD_BLACK;
constexpr const uint16_t kRedColor = EPD_RED;

// Just for debugging.
constexpr const bool kDrawBorders = false;

String GetDayOfWeekAbbrev(const String& dow) {
  return dow.substring(0, 3);
}

// Given "Saturday, November 17, 2019" return
// "November 17, 2019".
String GetMonthDayYear(const String& full_date) {
  int idx = full_date.indexOf(", ");
  if (idx < 0)
    return full_date;
  return full_date.substring(idx + 2);
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

const GFXglyph* GetGlyph(const GFXfont& font, char ch) {
  if (static_cast<uint8_t>(ch) < font.first)
    return nullptr;
  return &font.glyph[static_cast<uint8_t>(ch) - font.first];
}

Size GetCharSize(const GFXfont& font, char ch) {
  const GFXglyph* glyph = GetGlyph(font, ch);
  if (!glyph)
    return Size();
  return Size({glyph->width, glyph->height});
}

Point GetCharOffset(const GFXfont& font, char ch) {
  const GFXglyph* glyph = GetGlyph(font, ch);
  if (!glyph)
    return Point();
  return Point({glyph->xOffset, glyph->yOffset});
}

int StringWidth(const GFXfont& font, const String& str) {
  int width = 0;
  for (int i = 0; i < str.length(); i++) {
    const GFXglyph* glyph = GetGlyph(font, str[i]);
    if (glyph)
      width += glyph->xAdvance;
  }
  return width;
}

constexpr const GFXfont& kNormalFont = windows_command_prompt11pt7b;
constexpr const GFXfont& kLargeFont = LibreBaskerville_Bold18pt7b;

const int g_normal_font_height = GetCharSize(kNormalFont, 'W').height;
const int g_large_font_height = GetCharSize(kLargeFont, 'W').height;

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
  display_.setFont(&kNormalFont);
  if (kDrawBorders) {
    display_.drawRect(kTodayBounds.left(), kTodayBounds.top(),
                      kTodayBounds.width(), kTodayBounds.height(), kRedColor);
  }
  const int kMargin = 4;
  DrawString(status.day_of_week,
             Point({kMargin, kMargin + g_normal_font_height}), kBlackColor);
  DrawString(GetMonthDayYear(status.date_full),
             Point({kMargin, 2 * (kMargin + g_normal_font_height)}),
             kBlackColor);

  DrawString(status.alert_status,
             Point({kMargin + 8, 58 + g_normal_font_height}),
             status.AlertInEffect() ? kRedColor : kBlackColor);

  DrawAQIMeter({kTodayBounds.right() - 3 * kMargin - kAQIMeterSize.width -
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
  display_.setFont(&kNormalFont);
  const int kMargin = 4;
  DrawString(GetDayOfWeekAbbrev(status.day_of_week),
             Point({bounds.left() + kMargin,
                    bounds.top() + kMargin + g_normal_font_height}),
             kBlackColor);

  display_.setFont(&kLargeFont);
  String aqi_str = status.aqi_val != -1
                       ? String(status.aqi_val)
                       : Network::AQICategoryAbbrev(status.aqi_category);

  DrawString(aqi_str, Point({bounds.left() + kMargin,
                             bounds.top() + kMargin + g_normal_font_height +
                                 kMargin + g_large_font_height}),
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

}  // namespace spare_the_air
