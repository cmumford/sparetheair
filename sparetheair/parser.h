// Copyright 2019 Christopher Mumford
// This code is licensed under MIT license (see LICENSE for details)

#ifndef SPARETHEAIR_PARSER_H_
#define SPARETHEAIR_PARSER_H_

#include "status.h"

namespace spare_the_air {

struct RegionValues {
  String name;
  String aqi;
  String pollutant;
};

constexpr const int kMaxNumEntries = 6;

class Parser {
 public:
  // Extract the day of week from a string in one of two formats:
  //
  // 1. "Saturday, November 23, 2019"
  // 2. "BAAQMD Air Quality Forecast for Friday"
  //
  static String ExtractDayOfWeek(const String& str);
  static RegionValues ExtractRegionValues(const String& region_data,
                                          const String& region_name);
  static AQICategory ParseAQIName(const String& name);
  static AQICategory AQIValueToCategory(int value);
  static const char* AQICategoryAbbrev(AQICategory category);
  static void ParseAlert(const String& xmlString);
  static void ParseForecast(const String& xmlString);
  // Merge the alert response into the forecast response.
  static void MergeAlert();
  // Return the alert response status.
  static const Status& AlertStatus();
  // Index 0 is always today, and index 1 is tomorrow, etc.
  static const Status& status(int idx);
  // Return the forecast given the index from 0..kMaxNumEntries.
  static Status& forecast(int idx);
  // Clear all prior state for this class.
  static void Reset();
};

}  // namespace spare_the_air

#endif  // SPARETHEAIR_PARSER_H_
