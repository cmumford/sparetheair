#include <HTTPClient.h>
#include <TinyXML.h>

#include "sparetheair.h"

namespace sta {

namespace {

struct HttpFetchResult {
  int httpCode;
  String response;
};

const char* kAlertUrl = "http://www.baaqmd.gov/Feeds/AlertRSS.aspx";
const char* kForecastUrl = "http://www.baaqmd.gov/Feeds/AirForecastRSS.aspx";

uint8_t g_xml_parse_buffer[512];
int g_parse_forecast_idx = 0;
const int kMaxNumEntries = 5;
// The first one is today.
Status g_forecasts[kMaxNumEntries];

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
    g_forecasts[0].date_full = data;
  } else if (!strcasecmp(tag_name, "/rss/channel/item/description")) {
    g_forecasts[0].alert_status = data;
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
  if (!strcasecmp(tag_name, "/rss/channel/item/title")) {
    g_forecasts[g_parse_forecast_idx].date_full = data;
  }
}

}  // namespace

// static
void Status::ResetForTest() {
  *this = Status();
}

// static
int SpareTheAir::Fetch() {
  int err = FetchAlert();
  int ferr = FetchForecast();
  // Return the first error.
  return err ? err != HTTP_CODE_OK : ferr;
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
String SpareTheAir::ExtractDayOfWeek(const String& date_full) {
  int idx = date_full.indexOf(',');
  if (idx <= 0)
    return String();
  return date_full.substring(0, idx);
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
  return g_forecasts[idx];
}

void SpareTheAir::ResetForTest() {
  for (int i = 0; i < kMaxNumEntries; i++)
    g_forecasts[i].ResetForTest();
}

}  // namespace sta
