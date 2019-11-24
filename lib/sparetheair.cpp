// Copyright 2019 Christopher Mumford
// This code is licensed under MIT license (see LICENSE for details)

#include <HTTPClient.h>
#include <TinyXML.h>

#include "sparetheair.h"

namespace sta {

namespace {

struct HttpFetchResult {
  int httpCode;
  String response;
};

const char kAlertUrl[] = "http://www.baaqmd.gov/Feeds/AlertRSS.aspx";
const char kForecastUrl[] = "http://www.baaqmd.gov/Feeds/AirForecastRSS.aspx";
const char kRegion[] = "Santa Clara Valley";
const int kMaxNumEntries = 6;

// Buffer used by the XML parser.
uint8_t g_xml_parse_buffer[512];
// The index of the forecast status currently being written to by the
// XML parser callback.
int g_parse_forecast_idx = 0;
// The response from the alert url.
Status g_today;
// Empty status used for errors.
Status g_empty_status;
// The first one is today which is made up from both the alert request
// and the corresponding foreast entry.
Status g_forecasts[kMaxNumEntries];
// This is the index into |g_forecasts| that corresponds to the alert day
// (i,e today).
int g_today_idx = -1;

static_assert(kMaxNumEntries >= kNumStatusDays, "Too small");

HttpFetchResult DoHTTPGet(const char* url) {
  HttpFetchResult result;
  HTTPClient client;
  client.begin(url);
  result.httpCode = client.GET();
  if (result.httpCode == HTTP_CODE_OK)
    result.response = client.getString();
  return result;
}

void XML_Alertcallback(uint8_t status_flags,
                       char* tag_name,
                       uint16_t tag_name_len,
                       char* data,
                       uint16_t data_len) {
  if (!(status_flags & STATUS_TAG_TEXT))
    return;
  if (!strcasecmp(tag_name, "/rss/channel/item/date")) {
    g_today.date_full = data;
  } else if (!strcasecmp(tag_name, "/rss/channel/item/description")) {
    g_today.alert_status = data;
  }
}

void XML_ForecastCallback(uint8_t status_flags,
                          char* tag_name,
                          uint16_t tag_name_len,
                          char* data,
                          uint16_t data_len) {
  if ((status_flags & STATUS_TAG_TEXT) &&
      !strcasecmp(tag_name, "/rss/channel/item")) {
    g_parse_forecast_idx++;
  }
  if (!(status_flags & STATUS_TAG_TEXT))
    return;
  if (g_parse_forecast_idx >= kMaxNumEntries)
    return;
  Status forecast& = g_forecasts[g_parse_forecast_idx];
  if (!strcasecmp(tag_name, "/rss/channel/item/title")) {
    forecast.date_full = data;
  } else if (!strcasecmp(tag_name, "/rss/channel/item/description")) {
    RegionValues values = ExtractRegionValues(data, kRegion);
    AQICategory category = SpareTheAir::ParseAQIName(values.aqi);
    if (category == AQICategory::None) {
      forecast.aqi_val = atoi(values.aqi);
      forecast.aqi_category = AQIValueToCategory(values.aqi);
    } else {
      forecast.aqi_category = category;
    }
    forecast.pollutant = values.pollutant;
  }
}

}  // namespace

// static
void Status::Reset() {
  *this = Status();
}

// static
int SpareTheAir::Fetch() {
  Reset();
  int err = FetchAlert();
  if (!err) {
    err = FetchForecast();
    if (!err)
      MergeAlert();
  } else {
    FetchForecast();
  }

  return err;
}

// static
int SpareTheAir::FetchAlert() {
  HttpFetchResult result = DoHTTPGet(kAlertUrl);
  if (result.httpCode != HTTP_CODE_OK)
    return result.httpCode;

  ParseAlert(result.response);
  return HTTP_CODE_OK;
}

// static
void SpareTheAir::ParseAlert(const String& xmlString) {
  TinyXML xml;
  xml.init((uint8_t*)g_xml_parse_buffer, sizeof(g_xml_parse_buffer),
           &XML_Alertcallback);
  for (size_t i = 0; i < xmlString.length(); i++) {
    xml.processChar(xmlString[i]);
  }

  g_forecasts[0].day_of_week = ExtractDayOfWeek(g_forecasts[0].date_full);
}

// static
int SpareTheAir::FetchForecast() {
  HttpFetchResult result = DoHTTPGet(kForecastUrl);
  if (result.httpCode != HTTP_CODE_OK)
    return result.httpCode;

  ParseForecast(result.response);
  return HTTP_CODE_OK;
}

// static
void SpareTheAir::ParseForecast(const String& xmlString) {
  TinyXML xml;
  xml.init((uint8_t*)g_xml_parse_buffer, sizeof(g_xml_parse_buffer),
           &XML_ForecastCallback);
  for (size_t i = 0; i < xmlString.length(); i++) {
    xml.processChar(xmlString[i]);
  }
}

// static
String SpareTheAir::ExtractDayOfWeek(const String& str) {
  // This is the format used in the forecast item title
  int idx = str.indexOf("BAAQMD Air Quality Forecast for ");
  if (idx > 0)
    return title.substring(idx);
  idx = str.indexOf(',');
  if (idx <= 0)
    return String();
  return str.substring(0, idx);
}

// The region data is of the form:
//
// "Santa Clara Valley - AQI: 55, Pollutant: PM2.6"
//
// static
RegionValues SpareTheAir::ExtractRegionValues(const String& region_data,
                                              const String& region_name) {
  RegionValues values;

  int region_idx = region_data.indexOf(region_name);
  if (region_idx < 0)
    return values;

  values.name = region_name;
  int idx = region_data.indexOf("AQI: ", region_idx);
  if (idx > region_idx) {
    int end = region_data.indexOf(",", idx);
    const int kAqiLen = 5;  // Length of "AQI: "
    if (end > idx)
      values.aqi = region_data.substring(idx + kAqiLen, end);
  }

  idx = region_data.indexOf("Pollutant: ", region_idx);
  if (idx > region_idx) {
    int end = region_data.indexOf("\n", idx);
    const int kPollutantLen = 11;  // Length of "Pollutant: "
    if (end > idx) {
      // Not the last value.
      values.pollutant = region_data.substring(idx + kPollutantLen, end);
    } else {
      // the last value in the data string.
      values.pollutant = region_data.substring(idx + kPollutantLen);
    }
  }

  return values;
}

// static
const Status& SpareTheAir::status(int idx) {
  if (g_today_idx == -1)
    return idx == 0 ? g_today : g_empty;
  idx += g_today_idx;
  if (idx >= kMaxNumEntries)
    return g_empty;
  return g_forecasts[idx];
}

// static
void SpareTheAir::Reset() {
  g_today_idx = -1;
  for (int i = 0; i < kMaxNumEntries; i++)
    g_forecasts[i].Reset();
}

// static
AQICategory SpareTheAir::ParseAQIName(const String& name) {
  if (name == "Good")
    return AQICategory::Good;
  if (name == "Moderate")
    return AQICategory::Moderate;
  if (name == "Unhealthy for Sensitive Groups")
    return AQICategory::UnhealthyForSensitiveGroups;
  if (name == "Unhealthy")
    return AQICategory::Unhealthy;
  if (name == "Very Unhealthy")
    return AQICategory::VeryUnhealthy;
  if (name == "Hazardous")
    return AQICategory::Hazardous;
  return AQICategory::None;
}

// static
const char* SpareTheAir::AQICategoryAbbrev(AQICatetory category) {
  switch (category) {
    case AQICategory::Good:
      return "G";
    case AQICategory::Moderate:
      return "M";
    case AQICategory::UnhealthyForSensitiveGroups:
      return "USG";
    case AQICategory::Unhealthy:
      return "U";
    case AQICategory::VeryUnhealthy:
      return "VU";
    case AQICategory::Hazardous:
      return "H";
  }
  return "?";
}

// static
AQICategory SpareTheAir::AQIValueToCategory(int value) {
  if (value <= 50):
        return AQI::Good;
  if (value <= 100):
        return AQI::Moderate;
  if (value <= 150):
        return AQI::UnhealthyForSensitiveGroups;
  if (value <= 200):
        return AQI::Unhealthy;
  if (value <= 300):
        return AQI::VeryUnhealthy;
  return AQI::Hazardous;
}

// The forecast results may contain days in the past, so the actual alert
// day (today) may be somewhere in the middle of the forecast array. Find
// the matching day of week, and merge the values from the alert response
// into the corresponding forecast entry.
//
// static
void SpareTheAir::MergeAlert() {
  g_today_idx = -1;
  if (g_today.day_of_week == "")
    return;
  for (int i = 0; i < kMaxNumEntries; i++) {
    Status& forecast = g_forecasts[i];
    if (g_today.day_of_week == forecast.day_of_week) {
      forecast.alert_status = g_today.alert_status;
      forecast.date_full = g_today.date_full;
      g_today_idx = i;
      return;
    }
  }
}

}  // namespace sta
