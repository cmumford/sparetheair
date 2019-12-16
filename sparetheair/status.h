// Copyright 2019 Christopher Mumford
// This code is licensed under MIT license (see LICENSE for details)

#ifndef SPARETHEAIR_STATUS_H_
#define SPARETHEAIR_STATUS_H_

#include <Arduino.h>

namespace spare_the_air {

// Keep these in order from best to worst.
enum class AQICategory : int {
  Good = 0,
  Moderate = 1,
  UnhealthyForSensitiveGroups = 2,
  Unhealthy = 3,
  VeryUnhealthy = 4,
  Hazardous = 5,
  None = 9,
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

}  // namespace spare_the_air

#endif  // SPARETHEAIR_STATUS_H_
