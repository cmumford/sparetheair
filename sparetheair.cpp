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
const int kMaxNumEntries = 1 + sta::kNumForecastDays;
// The first one is today.
Status g_forecasts[kMaxNumEntries];

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
  // Serial.printf("Callback on tag %s\n", tag_name);
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
  Serial.printf("Callback on tag %s\n", tag_name);
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

int SpareTheAir::Fetch() {
  int err = FetchAlert();
  int ferr = FetchForecast();
  // Return the first error.
  return err ? err != HTTP_CODE_OK : ferr;
}

int SpareTheAir::FetchAlert() {
  HttpFetchResult result = DoHTTPGet(kAlertUrl);
  if (result.httpCode != HTTP_CODE_OK)
    return result.httpCode;

  TinyXML xml;
  xml.init((uint8_t*)g_xml_parse_buffer, sizeof(g_xml_parse_buffer),
           &XML_Alertcallback);
  for (size_t i = 0; i < result.response.length(); i++) {
    xml.processChar(result.response[i]);
  }

  int idx = g_forecasts[0].date_full.indexOf(',');
  if (idx > 0)
    g_forecasts[0].day_of_week = g_forecasts[0].date_full.substring(0, idx);
  return HTTP_CODE_OK;
}

int SpareTheAir::FetchForecast() {
  HttpFetchResult result = DoHTTPGet(kForecastUrl);
  if (result.httpCode != HTTP_CODE_OK)
    return result.httpCode;

  TinyXML xml;
  xml.init((uint8_t*)g_xml_parse_buffer, sizeof(g_xml_parse_buffer),
           &XML_ForecastCallback);
  for (size_t i = 0; i < result.response.length(); i++) {
    xml.processChar(result.response[i]);
  }

  return HTTP_CODE_OK;
}

}  // namespace sta
