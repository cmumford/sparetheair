// Copyright 2019 Christopher Mumford
// This code is licensed under MIT license (see LICENSE for details)

#ifndef SPARETHEAIR_NETWORK_H_
#define SPARETHEAIR_NETWORK_H_

namespace spare_the_air {

// Keep these in order from best to worst.
enum class AQICategory : int {
  None = 0,
  Good = 1,
  Moderate = 2,
  UnhealthyForSensitiveGroups = 3,
  Unhealthy = 4,
  VeryUnhealthy = 5,
  Hazardous = 6,
};

// Status for a date. This may be the current status or a forecast.
struct Status {
  // The alert status (e.g. "No Alert" or "Alert In Effect").
  // Will be empty for forecasts.
  String alert_status;

  // Full date (if available) (e.g. "Sunday, November 17, 2019").
  // Will be empty for forecasts.
  String date_full;

  // Full day of week. i.e. "Sunday".
  String day_of_week;

  // The numerical AQI value in the range (0, 500). May not be available
  // for all items. If not available will be set to -1.
  int aqi_val = -1;

  // Good, Moderate, etc.
  AQICategory aqi_category = AQICategory::None;

  // e.g. "PM2.5".
  String pollutant;

  bool AlertInEffect() const { return alert_status == "Alert In Effect"; }
  void Reset();
};

struct RegionValues {
  String name;
  String aqi;
  String pollutant;
};

// The maximum number of status days returned by Network::status().
const int kNumStatusDays = 4;

class Network {
 public:
  // Fetch the alert/forecasts from the network. Returns 0 if successful.
  static int Fetch();

  Network() = delete;
  ~Network() = delete;

  // Index 0 is always today, and index 1 is tomorrow, etc.
  static const Status& status(int idx);

  // Second public section is only for unit tests.
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
  // Return the alert response status.
  static const Status& AlertStatus();
  // Fetch and parse the alert from the network. Returns 0 upon success.
  static int FetchAlert();
  static void ParseAlert(const String& xmlString);
  // Fetch and parse the forecasts from the network. Returns 0 upon success.
  static int FetchForecast();
  static void ParseForecast(const String& xmlString);
  // Similar to status(), but returns raw forecast array values.
  static const Status& forecast(int idx);
  // Merge the alert response into the forecast response.
  static void MergeAlert();
  // Clear all prior state for this class.
  static void Reset();
};

}  // namespace spare_the_air

#endif  // SPARETHEAIR_NETWORK_H_
