// Copyright 2019 Christopher Mumford
// This code is licensed under MIT license (see LICENSE for details)

#include <ArduinoUnit.h>
#include <ArduinoUnitMock.h>

#include "display.h"
#include "parser.h"

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
    "<description>Alert In Effect</description></item></channel>\n"
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
    "Santa Clara Valley - AQI: Unhealthy for Sensitive Groups, Pollutant: "
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

using spare_the_air::Display;
using spare_the_air::Parser;

test(draw) {
  // Simulate doing a full forecast/alert fetch.
  Parser::Reset();
  Parser::ParseAlert(k_today_no_alert_response);
  Parser::ParseForecast(k_forecast_response);
  Parser::MergeAlert();

  Display display;
  display.Draw();
}

void setup() {
  Serial.begin(9600);
}

void loop() {
  Test::run();
}
