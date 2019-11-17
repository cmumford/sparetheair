#include <HTTPClient.h>
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

const char* kAlertUrl = "http://www.baaqmd.gov/Feeds/AlertRSS.aspx";
const char* kForecastUrl = "http://www.baaqmd.gov/Feeds/AirForecastRSS.aspx";
const Size kEPaperSize = {264, 176};
const Rectangle kEPaperBounds = {
    Point({0, 0}), Point({kEPaperSize.width - 1, kEPaperSize.height - 1})};
const Rectangle kTodayBounds = {Point({0, 0}),
                                Point({kEPaperBounds.right(), 88})};
const int kForecastWidth = kEPaperSize.width / 3;
// There are 3 forecast sections so 2 dividers between them.
const int kDividers[2] = {
    kEPaperSize.width * 1 / 3,
    kEPaperSize.width * 2 / 3,
};
const Rectangle kForecastBounds[3] = {
    {Point({0 * kForecastWidth, kTodayBounds.bottom()}),
     Point({kDividers[0] - 1, kEPaperBounds.bottom()})},
    {Point({kDividers[0], kTodayBounds.bottom()}),
     Point({kDividers[1] - 1, kEPaperBounds.bottom()})},
    {Point({kDividers[1], kTodayBounds.bottom()}),
     Point({kEPaperBounds.right(), kEPaperBounds.bottom()})},
};
char WiFiSSID[] = SECRET_SSID;
char WiFiPassword[] = SECRET_WIFI_PASSWORD;

// The blue LED pin.
const int kLEDPin = 2;

HttpFetchResult DoHTTPGet(const char* url) {
  HttpFetchResult result;
  HTTPClient client;
  client.begin(url);
  result.httpCode = client.GET();
  if (result.httpCode == HTTP_CODE_OK)
    result.response = client.getString();
  return result;
}

void ParseStatusString(const String& str, Status* status) {
  
}

Status FetchAlert() {
  Status status;
  HttpFetchResult result = DoHTTPGet(kAlertUrl);
  if (result.httpCode == HTTP_CODE_OK)
    ParseStatusString(result.response, &status);
  return status;
}

}  // namespace

void setup() {
  Serial.begin(115200);
  pinMode(kLEDPin, OUTPUT);
  Serial.printf("In setup");

  Serial.printf("Connecting to %s\n", WiFiSSID);
  WiFi.begin(WiFiSSID, WiFiPassword);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }

  Serial.printf("Connected to %s\n", WiFiSSID);
}

void loop() {
  Status alert = FetchAlert();
  Serial.println("Connecting to WiFi..");

  int i = 0;
  while (true) {
    Serial.printf("Message %d\n", i++);
    digitalWrite(kLEDPin, HIGH);
    delay(500);
    digitalWrite(kLEDPin, LOW);
    delay(500);
  }
}
