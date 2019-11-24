// Copyright 2019 Christopher Mumford
// This code is licensed under MIT license (see LICENSE for details)

#include <ArduinoUnit.h>
#include <ArduinoUnitMock.h>

#include "sparetheair.h"

const char k_today_no_alert_response[] =
    "﻿<?xml version=\"1.0\" encoding=\"utf-8\"?><rss version=\"2.0\">\n"
    "\n"
    "<channel>\n"
    "<title>Air Alerts for the San Francisco Bay Area</title>\n"
    "<link>http://www.sparetheair.org/</link>\n"
    "<description>Spare the Air Alert Status</description>\n"
    "<language>en</language>\n"
    "<lastBuildDate>Saturday, November 23, 2019 11:42</lastBuildDate>\n"
    "\n"
    "<item>\n"
    "<title>Spare the Air Status for Saturday, November 23, 2019</title>\n"
    "<date>Saturday, November 23, 2019</date>\n"
    "<description>No Alert</description></item></channel>\n"
    "</rss>";

const char k_forecast_response[] =
    "﻿<?xml version=\"1.0\" encoding=\"utf-8\"?><rss version=\"2.0\">\n"
    "\n"
    "<channel>\n"
    "<title>Bay Area Air Quality Management District Air Quality "
    "Forecast</title>\n"
    "<link>http://www.baaqmd.gov/</link>\n"
    "<description>Current air quality forecasts and alerts for the San "
    "Francisco Bay Area delivered using RSS. The information for this RSS feed "
    "is provided by the Bay Area Air Quality Management District and is "
    "updated at least once per weekday. Revisions to the forecast may be made "
    "at the discretion of BAAQMD. The Air Quality Forecast is comprised of an "
    "Air Quality Index and Pollutant Type forecast for each section of the "
    "District - North Counties, Coast and Central Bay, Eastern District, South "
    "and Central Bay, and Santa Clara Valley. Further explanation of the "
    "forecast can be found at www.baaqmd.gov. If you are using the feed on "
    "your web site, we request you maintain a link to "
    "www.baaqmd.gov.</description>\n"
    "<language>en</language>\n"
    "<lastBuildDate>Friday, November 22 at 9:50 AM</lastBuildDate>\n"
    "\n"
    "<image>\n"
    "<title>BAAQMD - Air Quality Forecast</title>\n"
    "<url>http://www.baaqmd.gov/images/logo-baaqmd-rss.gif</url>\n"
    "<link>http://www.baaqmd.gov</link>\n"
    "<width>144</width>\n"
    "<height>68</height>\n"
    "</image>\n"
    "\n"
    "<item>\n"
    "<title>BAAQMD Air Quality Forecast for Friday</title>\n"
    "<link>http://www.baaqmd.gov</link>\n"
    "<description>AQI and Pollutant Forecast for Friday by district.\n"
    "North Counties - AQI: 55, Pollutant: PM2.5.\n"
    "Coast and Central Bay - AQI: 57, Pollutant: PM2.5.\n"
    "Eastern District - AQI: 53, Pollutant: PM2.5\n"
    "South and Central Bay - AQI: 50, Pollutant: PM2.5\n"
    "Santa Clara Valley - AQI: 55, Pollutant: PM2.5</description></item>\n"
    "\n"
    "<item>\n"
    "<title>BAAQMD Air Quality Forecast for Saturday</title>\n"
    "<link>http://www.baaqmd.gov</link>\n"
    "<description>AQI and Pollutant Forecast for Saturday by district.\n"
    "North Counties - AQI: 61, Pollutant: PM2.5.\n"
    "Coast and Central Bay - AQI: 59, Pollutant: PM2.5.\n"
    "Eastern District - AQI: 61, Pollutant: PM2.5\n"
    "South and Central Bay - AQI: 50, Pollutant: PM2.5\n"
    "Santa Clara Valley - AQI: 63, Pollutant: PM2.5</description></item>\n"
    "\n"
    "<item>\n"
    "<title>BAAQMD Air Quality Forecast for Sunday</title>\n"
    "<link>http://www.baaqmd.gov</link>\n"
    "<description>AQI and Pollutant Forecast for Sunday by district.\n"
    "North Counties - AQI: 68, Pollutant: PM2.5.\n"
    "Coast and Central Bay - AQI: 63, Pollutant: PM2.5.\n"
    "Eastern District - AQI: 66, Pollutant: PM2.5\n"
    "South and Central Bay - AQI: 53, Pollutant: PM2.5\n"
    "Santa Clara Valley - AQI: 72, Pollutant: PM2.5</description></item>\n"
    "\n"
    "<item>\n"
    "<title>BAAQMD Air Quality Forecast for Monday</title>\n"
    "<link>http://www.baaqmd.gov</link>\n"
    "<description>AQI and Pollutant Forecast for Monday by district.\n"
    "North Counties - AQI: Moderate, Pollutant: PM2.5.\n"
    "Coast and Central Bay - AQI: Moderate, Pollutant: PM2.5.\n"
    "Eastern District - AQI: Good, Pollutant: PM2.5\n"
    "South and Central Bay - AQI: Good, Pollutant: OZONE\n"
    "Santa Clara Valley - AQI: Moderate, Pollutant: "
    "PM2.5</description></item>\n"
    "\n"
    "<item>\n"
    "<title>BAAQMD Air Quality Forecast for Tuesday</title>\n"
    "<link>http://www.baaqmd.gov</link>\n"
    "<description>AQI and Pollutant Forecast for Tuesday by district.\n"
    "North Counties - AQI: Good, Pollutant: PM2.5.\n"
    "Coast and Central Bay - AQI: Moderate, Pollutant: PM2.5.\n"
    "Eastern District - AQI: Good, Pollutant: PM2.5\n"
    "South and Central Bay - AQI: Good, Pollutant: PM2.5\n"
    "Santa Clara Valley - AQI: Moderate, Pollutant: "
    "PM2.5</description></item>\n"
    "\n"
    "</channel>\n"
    "</rss>";

const char kRegionData[] =
    "AQI and Pollutant Forecast for Friday by district.\n"
    "North Counties - AQI: 55, Pollutant: PM2.5.\n"
    "Coast and Central Bay - AQI: 57, Pollutant: PM2.5.\n"
    "Eastern District - AQI: 53, Pollutant: PM2.5\n"
    "South and Central Bay - AQI: 50, Pollutant: PM2.5\n"
    "Santa Clara Valley - AQI: 55, Pollutant: PM2.6";

using sta::RegionValues;
using sta::SpareTheAir;
using sta::Status;
using sta::AQICategory;

// ArduinoUnit can't handle enumerations, so cast to integer.
int CatToInt(AQICategory cat) {
  return static_cast<int>(cat);
}

test(dayOfWeek) {
  SpareTheAir::Reset();
  assertEqual(
      SpareTheAir::ExtractDayOfWeek("BAAQMD Air Quality Forecast for Friday"),
      "Friday");
  assertEqual(SpareTheAir::ExtractDayOfWeek("Saturday, November 23, 2019"),
              "Saturday");
  assertEqual(SpareTheAir::ExtractDayOfWeek("Invalid date."), "");
  assertEqual(SpareTheAir::ExtractDayOfWeek(""), "");
}

test(parseAQIName) {
  assertEqual(CatToInt(SpareTheAir::ParseAQIName("Good")),
              CatToInt(AQICategory::Good));
  assertEqual(CatToInt(SpareTheAir::ParseAQIName("Moderate")),
              CatToInt(AQICategory::Moderate));
  assertEqual(
      CatToInt(SpareTheAir::ParseAQIName("Unhealthy for Sensitive Groups")),
      CatToInt(AQICategory::UnhealthyForSensitiveGroups));
  assertEqual(CatToInt(SpareTheAir::ParseAQIName("Unhealthy")),
              CatToInt(AQICategory::Unhealthy));
  assertEqual(CatToInt(SpareTheAir::ParseAQIName("Very Unhealthy")),
              CatToInt(AQICategory::VeryUnhealthy));
  assertEqual(CatToInt(SpareTheAir::ParseAQIName("Hazardous")),
              CatToInt(AQICategory::Hazardous));

  assertEqual(CatToInt(SpareTheAir::ParseAQIName("InvalidText")),
              CatToInt(AQICategory::None));
  assertEqual(CatToInt(SpareTheAir::ParseAQIName("")),
              CatToInt(AQICategory::None));
}

test(parseAQICategoryValue) {
  assertEqual(CatToInt(SpareTheAir::AQIValueToCategory(0)),
              CatToInt(AQICategory::Good));
  assertEqual(CatToInt(SpareTheAir::AQIValueToCategory(10)),
              CatToInt(AQICategory::Good));
  assertEqual(CatToInt(SpareTheAir::AQIValueToCategory(51)),
              CatToInt(AQICategory::Moderate));
  assertEqual(CatToInt(SpareTheAir::AQIValueToCategory(100)),
              CatToInt(AQICategory::Moderate));
  assertEqual(CatToInt(SpareTheAir::AQIValueToCategory(101)),
              CatToInt(AQICategory::UnhealthyForSensitiveGroups));
  assertEqual(CatToInt(SpareTheAir::AQIValueToCategory(150)),
              CatToInt(AQICategory::UnhealthyForSensitiveGroups));
  assertEqual(CatToInt(SpareTheAir::AQIValueToCategory(151)),
              CatToInt(AQICategory::Unhealthy));
  assertEqual(CatToInt(SpareTheAir::AQIValueToCategory(200)),
              CatToInt(AQICategory::Unhealthy));
  assertEqual(CatToInt(SpareTheAir::AQIValueToCategory(201)),
              CatToInt(AQICategory::VeryUnhealthy));
  assertEqual(CatToInt(SpareTheAir::AQIValueToCategory(300)),
              CatToInt(AQICategory::VeryUnhealthy));
  assertEqual(CatToInt(SpareTheAir::AQIValueToCategory(301)),
              CatToInt(AQICategory::Hazardous));
  assertEqual(CatToInt(SpareTheAir::AQIValueToCategory(600)),
              CatToInt(AQICategory::Hazardous));

  assertEqual(CatToInt(SpareTheAir::AQIValueToCategory(-1)),
              CatToInt(AQICategory::None));
}

test(aqiCategoryAbbrev) {
  assertEqual(SpareTheAir::AQICategoryAbbrev(AQICategory::Good), "G");
  assertEqual(SpareTheAir::AQICategoryAbbrev(AQICategory::Moderate), "M");
  assertEqual(
      SpareTheAir::AQICategoryAbbrev(AQICategory::UnhealthyForSensitiveGroups),
      "USG");
  assertEqual(SpareTheAir::AQICategoryAbbrev(AQICategory::Unhealthy), "U");
  assertEqual(SpareTheAir::AQICategoryAbbrev(AQICategory::VeryUnhealthy), "VU");
  assertEqual(SpareTheAir::AQICategoryAbbrev(AQICategory::Hazardous), "H");
  assertEqual(SpareTheAir::AQICategoryAbbrev(AQICategory::None), "?");
}

test(regionValues) {
  SpareTheAir::Reset();

  RegionValues values =
      SpareTheAir::ExtractRegionValues(kRegionData, "Eastern District");
  assertEqual(values.name, "Eastern District");
  assertEqual(values.aqi, "53");
  assertEqual(values.pollutant, "PM2.5");

  values = SpareTheAir::ExtractRegionValues(kRegionData, "Santa Clara Valley");
  assertEqual(values.name, "Santa Clara Valley");
  assertEqual(values.aqi, "55");
  assertEqual(values.pollutant, "PM2.6");

  values = SpareTheAir::ExtractRegionValues(kRegionData, "InvalidName");
  assertEqual(values.name, "");
  assertEqual(values.aqi, "");
  assertEqual(values.pollutant, "");
}

test(parseAlert) {
  SpareTheAir::Reset();
  SpareTheAir::ParseAlert(k_today_no_alert_response);

  const Status& today = SpareTheAir::AlertStatus();
  assertEqual(today.alert_status, "No Alert");
  assertEqual(today.date_full, "Saturday, November 23, 2019");
  assertEqual(today.day_of_week, "Saturday");
  // The following values only come from the forecast.
  assertEqual(today.aqi_val, 0);
  assertEqual(CatToInt(today.aqi_category), CatToInt(AQICategory::None));
  assertEqual(today.pollutant, "");
}

test(parseForecast) {
  SpareTheAir::Reset();
  SpareTheAir::ParseForecast(k_forecast_response);

  const Status& forecast = SpareTheAir::forecast(0);
  // First two only come from the alert.
  assertEqual(forecast.alert_status, "");
  assertEqual(forecast.date_full, "");
  assertEqual(forecast.day_of_week, "Friday");
  assertEqual(forecast.aqi_val, 55);
  assertEqual(CatToInt(forecast.aqi_category), CatToInt(AQICategory::Moderate));
  assertEqual(forecast.pollutant, "PM2.5");
}

test(fullFetch) {
  SpareTheAir::Reset();

  // Sumulate doing a full forecast/alert fetch.
  SpareTheAir::ParseAlert(k_today_no_alert_response);
  SpareTheAir::ParseForecast(k_forecast_response);
  SpareTheAir::MergeAlert();

  const Status& today = SpareTheAir::status(0);
  assertEqual(today.alert_status, "No Alert");
  assertEqual(today.date_full, "Saturday, November 23, 2019");
  assertEqual(today.day_of_week, "Saturday");
  // The following values only come from the forecast.
  assertEqual(today.aqi_val, 55);
  assertEqual(CatToInt(today.aqi_category), CatToInt(AQICategory::Moderate));
  assertEqual(today.pollutant, "PM2.5");
}

test(failedFetch) {
  SpareTheAir::Reset();
  const Status& today = SpareTheAir::status(0);
  assertEqual(today.alert_status, "");
  assertEqual(today.date_full, "");
  assertEqual(today.day_of_week, "");
  assertEqual(today.aqi_val, 0);
  assertEqual(CatToInt(today.aqi_category), CatToInt(AQICategory::None));
  assertEqual(today.pollutant, "");
}

void setup() {
  Serial.begin(9600);
}

void loop() {
  Test::run();
}
