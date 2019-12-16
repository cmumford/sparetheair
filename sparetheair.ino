// Copyright 2019 Christopher Mumford
// This code is licensed under MIT license (see LICENSE for details)

#include <WiFi.h>

#include "arduino_secrets.h"
#include "sparetheair/display.cpp"
#include "sparetheair/display.h"
#include "sparetheair/network.cpp"
#include "sparetheair/network.h"

using spare_the_air::Display;
using spare_the_air::Network;

namespace {

constexpr const char kWiFiSSID[] = SECRET_SSID;
constexpr const char kWiFiPassword[] = SECRET_WIFI_PASSWORD;

// The blue LED pin.
const int kLEDPin = 2;

RTC_DATA_ATTR int g_boot_count = 0;

}  // namespace

// Connect to WiFi. Will flash the LED while waiting to connect.
// Return a WiFi.status() value where WL_CONNECTED indicates
// a successful connection to the WiFi network.
int ConnectWiFi() {
  Serial.printf("Connecting to %s\n", kWiFiSSID);
  WiFi.begin(kWiFiSSID, kWiFiPassword);

  int led_state = HIGH;
  digitalWrite(kLEDPin, led_state);
  const int kAttemptSeconds = 10;
  const int kNumWaits = kAttemptSeconds * 4;  // 250 msec sleeps.
  for (int i = 0; i < kNumWaits && WiFi.status() != WL_CONNECTED; i++) {
    delay(250);
    led_state = led_state == HIGH ? LOW : HIGH;
    digitalWrite(kLEDPin, led_state);
  }
  Serial.printf("Connected to %s\n", kWiFiSSID);

  return WiFi.status();
}

int DisconnectWiFi() {
  // Can this be implemented?
  // WiFi.end();
  return 0;
}

void DrawStatus(int error) {
  Display display;
  if (error != HTTP_CODE_OK) {
    display.DrawError(error);
    return;
  }
  display.Draw();
}

int FetchStatus() {
  Serial.println("Fetching Status");
  digitalWrite(kLEDPin, HIGH);
  int err = ConnectWiFi();
  if (err != WL_CONNECTED) {
    digitalWrite(kLEDPin, LOW);
    Serial.printf("Error %d connecting to %s\n", err, kWiFiSSID);
    DrawStatus(err);
    return err;
  }
  digitalWrite(kLEDPin, HIGH);
  Network network;
  err = network.Fetch();
  DisconnectWiFi();
  DrawStatus(err);
  Serial.println("Successfully refreshed status");
  digitalWrite(kLEDPin, LOW);
  return 0;
}

void setup() {
  Serial.begin(115200);

  ++g_boot_count;
  Serial.println("Boot number: " + String(g_boot_count));

  FetchStatus();

  // Sleep for four hours.
  const uint64_t kSleepTimeUsec = 4 * 3600 * 1000 * 1000;
  esp_sleep_enable_timer_wakeup(kSleepTimeUsec);

  Serial.println("Going to sleep now");
  delay(1000);
  Serial.flush();
  esp_deep_sleep_start();

  pinMode(kLEDPin, OUTPUT);
}

void loop() {
  // This will never be called.
}
