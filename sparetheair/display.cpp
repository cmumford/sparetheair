// Copyright 2019 Christopher Mumford
// This code is licensed under MIT license (see LICENSE for details)

#include "aqi_meter_image.h"
#include "display.h"
#include "font_base.h"
#include "font_large.h"
#include "font_medium.h"
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
constexpr const int kForecastWidth = kEPaperSize.width / 3;

// There are 3 forecast sections so 2 dividers between them.
constexpr const int kDividers[kNumStatusDays - 1] = {
    kEPaperSize.width * 1 / 3,
    kEPaperSize.width * 2 / 3,
};

constexpr const uint16_t kWhite = EPD_WHITE;
constexpr const uint16_t kBlack = EPD_BLACK;
constexpr const uint16_t kRed = EPD_RED;
constexpr const uint16_t kTransparent = 99;

constexpr const GFXfont& kNormalFont = windows_command_prompt11pt7b;
constexpr const GFXfont& kMediumFont = LibreBaskerville_Bold13pt7b;
constexpr const GFXfont& kLargeFont = LibreBaskerville_Bold18pt7b;

constexpr const int kArrowWidth = 5;

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
  switch (pixel) {
    case 3:
      return kWhite;
    case 2:
      return kBlack;
    case 1:
      return kRed;
    default:
      return kTransparent;
  }
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

int StringWidth(const GFXfont& font, const String& str) {
  int width = 0;
  for (int i = 0; i < str.length(); i++) {
    const GFXglyph* glyph = GetGlyph(font, str[i]);
    if (glyph)
      width += glyph->xAdvance;
  }
  return width;
}

const Rectangle kTodayBounds = {Point({0, 0}),
                                Point({kEPaperBounds.right(), 88})};
const Rectangle kForecastBounds[kNumStatusDays - 1] = {
    {Point({0 * kForecastWidth, kTodayBounds.bottom()}),
     Point({kDividers[0] - 1, kEPaperBounds.bottom()})},
    {Point({kDividers[0], kTodayBounds.bottom()}),
     Point({kDividers[1] - 1, kEPaperBounds.bottom()})},
    {Point({kDividers[1], kTodayBounds.bottom()}),
     Point({kEPaperSize.width - 1, kEPaperBounds.bottom()})},
};

const int kNormalFontHeight = GetCharSize(kNormalFont, 'W').height;

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

void Display::DrawError(int error) {
  if (!display_begun_) {
    display_.begin();
    display_begun_ = true;
  }
  display_.clearBuffer();
  const String msg = String("Error retrieving status: ") + String(error);

  display_.setFont(&kLargeFont);
  DrawString(msg, Point({4, 50}), kBlack);

  display_.display();
}

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

void Display::DrawImage(const Point tl,
                        const uint8_t* pixels,
                        const Size& size) {
  for (int y = 0; y < size.height; y++) {
    for (int x = 0; x < size.width; x++) {
      uint8_t color = GetPixelColor(*pixels++);
      if (color != kTransparent)
        display_.writePixel(tl.x + x, tl.y + y, color);
    }
  }
}

void Display::DrawLogo() {
  const int kMargin = 4;
  DrawImage(Point({kEPaperSize.width - kLogoSize.width - kMargin, 0}), kLogo,
            kLogoSize);
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
                      kTodayBounds.width(), kTodayBounds.height(), kRed);
  }
  if (false && status.AlertInEffect()) {
    display_.fillRect(kTodayBounds.left(), kTodayBounds.top(),
                      kTodayBounds.width(), kTodayBounds.height(), kRed);
  }
  const int kMargin = 4;
  DrawString(status.day_of_week, Point({kMargin, kMargin + kNormalFontHeight}),
             kBlack);
  DrawString(GetMonthDayYear(status.date_full),
             Point({kMargin, 2 * (kMargin + kNormalFontHeight)}), kBlack);

  display_.setFont(&kMediumFont);
  DrawString(status.alert_status, Point({kMargin + 8, 58 + kNormalFontHeight}),
             kBlack);

  DrawAQIMeter({kTodayBounds.right() - 3 * kMargin - kAQIMeterSize.width -
                    kLogoSize.width,
                20},
               status.aqi_category);
}

void Display::DrawForecastLines() {
  // Draw the horizontal line above the forecast section.
  display_.drawLine(kTodayBounds.left(), kTodayBounds.bottom(),
                    kTodayBounds.right(), kTodayBounds.bottom(), kBlack);
  // Draw the vertical lines dividing the forecast days.
  for (int i = 0; i < kNumStatusDays - 1; i++) {
    const Rectangle fr = kForecastBounds[i];
    display_.drawLine(fr.right(), fr.top(), fr.right(), fr.bottom(), kBlack);
  }
}

void Display::DrawForecast(const Status& status, const Rectangle& bounds) {
  if (kDrawBorders) {
    display_.drawRect(bounds.left(), bounds.top(), bounds.width(),
                      bounds.height(), kRed);
  }
  display_.setFont(&kNormalFont);
  const int kMargin = 4;
  int string_y = bounds.top() + kMargin + kNormalFontHeight;
  DrawString(GetDayOfWeekAbbrev(status.day_of_week),
             Point({bounds.left() + kMargin, string_y}), kBlack);

  const String aqi_str = status.aqi_val != -1
                             ? String(status.aqi_val)
                             : Network::AQICategoryAbbrev(status.aqi_category);
  const GFXfont& kAQIFont = aqi_str.length() > 2 ? kMediumFont : kLargeFont;

  const Rectangle aqi_bounds(
      {bounds.left(), string_y,
       bounds.right() - kAQIMeterSize.width - kMargin - kArrowWidth,
       bounds.bottom()});
  const Point tl({(aqi_bounds.left() + aqi_bounds.right() -
                   StringWidth(kAQIFont, aqi_str)) /
                      2,
                  (aqi_bounds.top() + aqi_bounds.bottom() +
                   GetCharSize(kAQIFont, 'W').height) /
                      2});
  display_.setFont(&kAQIFont);
  DrawString(aqi_str, tl, kBlack);

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
  for (int i = 0; i < kArrowWidth; i++) {
    for (int y = tip.y - i; y <= tip.y + i; y++) {
      display_.writePixel(tip.x - i, y, color);
    }
  }
}

void Display::DrawAQIMeter(const Point& tl, AQICategory category) {
  DrawImage(tl, kAQIMeterImage, kAQIMeterSize);

  if (category == AQICategory::None)
    return;

  // Now draw the little arrow.
  const int kMeterBlockHeight = kAQIMeterSize.width;
  int int_val = CatToInt(category);
  int y = tl.y + kAQIMeterSize.height - int_val * kMeterBlockHeight -
          kMeterBlockHeight / 2;
  DrawArrow({tl.x - 2, y}, kBlack);
}

}  // namespace spare_the_air
