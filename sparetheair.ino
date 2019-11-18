#include <HTTPClient.h>
#include <TinyXML.h>
#include <WiFi.h>

#include "secrets.h"

namespace {

struct Point {
  int x;
  int y;
};

struct Size {
  int width;
  int height;
};

struct Rectangle {
  Point tl;
  Point br;

  int right() const { return br.x; }
  int bottom() const { return br.y; }
};

// Status for a given date. This may be the current status
// or a forecast.
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
  int aqi_val;

  // One of ("Good", "Moderate", "Unhealthy for Sensitive Groups",
  //         "Unhealthy", "Very Unhealthy", "Hazardous").
  String aqi_name;

  // e.g. "PM2.5".
  String pollutant;

  bool AlertInEffect() const { return alert_status == "Alert In Effect"; }
};

struct HttpFetchResult {
  int httpCode;
  String response;
};

struct RegionValues {
  String name;
  String aqi;
  String pollutant;
};

const char* kAlertUrl = "http://www.baaqmd.gov/Feeds/AlertRSS.aspx";
const char* kForecastUrl = "http://www.baaqmd.gov/Feeds/AirForecastRSS.aspx";
const Size kEPaperSize = {264, 176};
const int kNumForecastDays = 4;
const Rectangle kEPaperBounds = {
    Point({0, 0}), Point({kEPaperSize.width - 1, kEPaperSize.height - 1})};
const Rectangle kTodayBounds = {Point({0, 0}),
                                Point({kEPaperBounds.right(), 88})};
const int kForecastWidth = kEPaperSize.width / 3;
// There are 4 forecast sections so 3 dividers between them.
const int kDividers[kNumForecastDays - 1] = {
    kEPaperSize.width * 1 / 4,
    kEPaperSize.width * 2 / 4,
    kEPaperSize.width * 3 / 4,
};
const Rectangle kForecastBounds[kNumForecastDays] = {
    {Point({0 * kForecastWidth, kTodayBounds.bottom()}),
     Point({kDividers[0] - 1, kEPaperBounds.bottom()})},
    {Point({kDividers[0], kTodayBounds.bottom()}),
     Point({kDividers[1] - 1, kEPaperBounds.bottom()})},
    {Point({kDividers[1], kTodayBounds.bottom()}),
     Point({kDividers[2] - 1, kEPaperBounds.bottom()})},
    {Point({kDividers[2], kTodayBounds.bottom()}),
     Point({kEPaperBounds.right(), kEPaperBounds.bottom()})},
};
char kWiFiSSID[] = SECRET_SSID;
char kWiFiPassword[] = SECRET_WIFI_PASSWORD;

// The blue LED pin.
const int kLEDPin = 2;

uint8_t g_xml_parse_buffer[512];
int g_parse_forecast_idx = 0;
const int kMaxNumEntries = 1 + kNumForecastDays;
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

int FetchAlert() {
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

void XML_ForecastCallback(uint8_t status_flags,
                          char* tag_name,
                          uint16_t tag_name_len,
                          char* data,
                          uint16_t data_len) {
  Serial.printf("Callback on tag %s\n", tag_name);
  if ((status_flags & STATUS_TAG_TEXT) && !strcasecmp(tag_name, "/rss/channel/item")) {
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

int FetchForecast() {
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

}  // namespace

void setup() {
  Serial.begin(115200);
  pinMode(kLEDPin, OUTPUT);

  Serial.printf("Connecting to %s\n", kWiFiSSID);
  WiFi.begin(kWiFiSSID, kWiFiPassword);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }

  Serial.printf("Connected to %s\n", kWiFiSSID);
}

void loop() {
  int status = FetchAlert();
  status = FetchForecast();

  int i = 0;
  while (true) {
    // Serial.printf("Message %d\n", i++);
    digitalWrite(kLEDPin, HIGH);
    delay(500);
    digitalWrite(kLEDPin, LOW);
    delay(500);
  }
}
