#include <WiFi.h>

#include "secrets.h"
#include "sparetheair.h"

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

const Size kEPaperSize = {264, 176};

const Rectangle kEPaperBounds = {
    Point({0, 0}), Point({kEPaperSize.width - 1, kEPaperSize.height - 1})};
const Rectangle kTodayBounds = {Point({0, 0}),
                                Point({kEPaperBounds.right(), 88})};
const int kForecastWidth = kEPaperSize.width / 3;
// There are 4 forecast sections so 3 dividers between them.
const int kDividers[sta::kNumForecastDays - 1] = {
    kEPaperSize.width * 1 / 4,
    kEPaperSize.width * 2 / 4,
    kEPaperSize.width * 3 / 4,
};
const Rectangle kForecastBounds[sta::kNumForecastDays] = {
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
  sta::SpareTheAir sta;
  int status = sta.Fetch();

  int i = 0;
  while (true) {
    // Serial.printf("Message %d\n", i++);
    digitalWrite(kLEDPin, HIGH);
    delay(500);
    digitalWrite(kLEDPin, LOW);
    delay(500);
  }
}
