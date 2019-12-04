// Copyright 2019 Christopher Mumford
// This code is licensed under MIT license (see LICENSE for details)

#include <ArduinoUnit.h>
#include <ArduinoUnitMock.h>

#include "display.h"
#include "network.h"

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

using spare_the_air::AQICategory;
using spare_the_air::Display;
using spare_the_air::Network;
using spare_the_air::RegionValues;
using spare_the_air::Status;

// ArduinoUnit can't handle enumerations, so cast to integer.
int CatToInt(AQICategory cat) {
  return static_cast<int>(cat);
}

test(dayOfWeek) {
  Network::Reset();
  assertEqual(
      Network::ExtractDayOfWeek("BAAQMD Air Quality Forecast for Friday"),
      "Friday");
  assertEqual(Network::ExtractDayOfWeek("Saturday, November 23, 2019"),
              "Saturday");
  assertEqual(Network::ExtractDayOfWeek("Invalid date."), "");
  assertEqual(Network::ExtractDayOfWeek(""), "");
  assertEqual(Network::ExtractDayOfWeek("BAAQMD Air Quality Forecast for "),
              "");
}

test(parseAQIName) {
  assertEqual(CatToInt(Network::ParseAQIName("Good")),
              CatToInt(AQICategory::Good));
  assertEqual(CatToInt(Network::ParseAQIName("Moderate")),
              CatToInt(AQICategory::Moderate));
  assertEqual(CatToInt(Network::ParseAQIName("Unhealthy for Sensitive Groups")),
              CatToInt(AQICategory::UnhealthyForSensitiveGroups));
  assertEqual(CatToInt(Network::ParseAQIName("Unhealthy")),
              CatToInt(AQICategory::Unhealthy));
  assertEqual(CatToInt(Network::ParseAQIName("Very Unhealthy")),
              CatToInt(AQICategory::VeryUnhealthy));
  assertEqual(CatToInt(Network::ParseAQIName("Hazardous")),
              CatToInt(AQICategory::Hazardous));

  assertEqual(CatToInt(Network::ParseAQIName("InvalidText")),
              CatToInt(AQICategory::None));
  assertEqual(CatToInt(Network::ParseAQIName("")), CatToInt(AQICategory::None));
}

test(parseAQICategoryValue) {
  assertEqual(CatToInt(Network::AQIValueToCategory(0)),
              CatToInt(AQICategory::Good));
  assertEqual(CatToInt(Network::AQIValueToCategory(10)),
              CatToInt(AQICategory::Good));
  assertEqual(CatToInt(Network::AQIValueToCategory(51)),
              CatToInt(AQICategory::Moderate));
  assertEqual(CatToInt(Network::AQIValueToCategory(100)),
              CatToInt(AQICategory::Moderate));
  assertEqual(CatToInt(Network::AQIValueToCategory(101)),
              CatToInt(AQICategory::UnhealthyForSensitiveGroups));
  assertEqual(CatToInt(Network::AQIValueToCategory(150)),
              CatToInt(AQICategory::UnhealthyForSensitiveGroups));
  assertEqual(CatToInt(Network::AQIValueToCategory(151)),
              CatToInt(AQICategory::Unhealthy));
  assertEqual(CatToInt(Network::AQIValueToCategory(200)),
              CatToInt(AQICategory::Unhealthy));
  assertEqual(CatToInt(Network::AQIValueToCategory(201)),
              CatToInt(AQICategory::VeryUnhealthy));
  assertEqual(CatToInt(Network::AQIValueToCategory(300)),
              CatToInt(AQICategory::VeryUnhealthy));
  assertEqual(CatToInt(Network::AQIValueToCategory(301)),
              CatToInt(AQICategory::Hazardous));
  assertEqual(CatToInt(Network::AQIValueToCategory(600)),
              CatToInt(AQICategory::Hazardous));

  assertEqual(CatToInt(Network::AQIValueToCategory(-1)),
              CatToInt(AQICategory::None));
}

test(aqiCategoryAbbrev) {
  assertEqual(Network::AQICategoryAbbrev(AQICategory::Good), "G");
  assertEqual(Network::AQICategoryAbbrev(AQICategory::Moderate), "M");
  assertEqual(
      Network::AQICategoryAbbrev(AQICategory::UnhealthyForSensitiveGroups),
      "USG");
  assertEqual(Network::AQICategoryAbbrev(AQICategory::Unhealthy), "U");
  assertEqual(Network::AQICategoryAbbrev(AQICategory::VeryUnhealthy), "VU");
  assertEqual(Network::AQICategoryAbbrev(AQICategory::Hazardous), "H");
  assertEqual(Network::AQICategoryAbbrev(AQICategory::None), "?");
}

test(regionValues) {
  Network::Reset();

  RegionValues values =
      Network::ExtractRegionValues(kRegionData, "Eastern District");
  assertEqual(values.name, "Eastern District");
  assertEqual(values.aqi, "53");
  assertEqual(values.pollutant, "PM2.5");

  values = Network::ExtractRegionValues(kRegionData, "Santa Clara Valley");
  assertEqual(values.name, "Santa Clara Valley");
  assertEqual(values.aqi, "55");
  assertEqual(values.pollutant, "PM2.6");

  values = Network::ExtractRegionValues(kRegionData, "InvalidName");
  assertEqual(values.name, "");
  assertEqual(values.aqi, "");
  assertEqual(values.pollutant, "");
}

test(parseAlert) {
  Network::Reset();
  Network::ParseAlert(k_today_no_alert_response);

  const Status& today = Network::AlertStatus();
  assertEqual(today.alert_status, "No Alert");
  assertEqual(today.date_full, "Saturday, November 23, 2019");
  assertEqual(today.day_of_week, "Saturday");
  // The following values only come from the forecast.
  assertEqual(today.aqi_val, -1);
  assertEqual(CatToInt(today.aqi_category), CatToInt(AQICategory::None));
  assertEqual(today.pollutant, "");
}

test(parseForecast) {
  Network::Reset();
  Network::ParseForecast(k_forecast_response);

  const Status& first = Network::forecast(0);
  // First two only come from the alert.
  assertEqual(first.alert_status, "");
  assertEqual(first.date_full, "BAAQMD Air Quality Forecast for Friday");
  assertEqual(first.day_of_week, "Friday");
  assertEqual(first.aqi_val, 55);
  assertEqual(CatToInt(first.aqi_category), CatToInt(AQICategory::Moderate));
  assertEqual(first.pollutant, "PM2.5");

  const Status& second = Network::forecast(1);
  assertEqual(second.alert_status, "");
  assertEqual(second.date_full, "BAAQMD Air Quality Forecast for Saturday");
  assertEqual(second.day_of_week, "Saturday");
  assertEqual(second.aqi_val, 63);
  assertEqual(CatToInt(second.aqi_category), CatToInt(AQICategory::Moderate));
  assertEqual(second.pollutant, "PM2.5");
}

test(fullFetch) {
  Network::Reset();

  // Simulate doing a full forecast/alert fetch.
  Network::ParseAlert(k_today_no_alert_response);
  Network::ParseForecast(k_forecast_response);
  Network::MergeAlert();

  const Status& today = Network::status(0);
  assertEqual(today.alert_status, "No Alert");
  assertEqual(today.date_full, "Saturday, November 23, 2019");
  assertEqual(today.day_of_week, "Saturday");
  // The following values only come from the forecast.
  assertEqual(today.aqi_val, 63);
  assertEqual(CatToInt(today.aqi_category), CatToInt(AQICategory::Moderate));
  assertEqual(today.pollutant, "PM2.5");
}

test(failedFetch) {
  Network::Reset();
  const Status& today = Network::status(0);
  assertEqual(today.alert_status, "");
  assertEqual(today.date_full, "");
  assertEqual(today.day_of_week, "");
  assertEqual(today.aqi_val, -1);
  assertEqual(CatToInt(today.aqi_category), CatToInt(AQICategory::None));
  assertEqual(today.pollutant, "");
}

test(draw) {
  Network::Reset();

  // Simulate doing a full forecast/alert fetch.
  Network::ParseAlert(k_today_no_alert_response);
  Network::ParseForecast(k_forecast_response);
  Network::MergeAlert();

  Display display;
  display.Draw();
}

void setup() {
  Serial.begin(9600);
}

void loop() {
  Test::run();
}
