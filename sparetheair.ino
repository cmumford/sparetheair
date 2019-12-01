// Copyright 2019 Christopher Mumford
// This code is licensed under MIT license (see LICENSE for details)

#include <WiFi.h>

#include "arduino_secrets.h"
#include "sparetheair/network.cpp"
#include "sparetheair/network.h"

using sta::Network;

namespace {

char kWiFiSSID[] = SECRET_SSID;
char kWiFiPassword[] = SECRET_WIFI_PASSWORD;

// The blue LED pin.
const int kLEDPin = 2;

}  // namespace

void setup() {
  Serial.begin(115200);
  pinMode(kLEDPin, OUTPUT);
}

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

void DrawStatus() {}

int FetchStatus() {
  Serial.println("Fetching Status");
  digitalWrite(kLEDPin, HIGH);
  int err = ConnectWiFi();
  if (err != WL_CONNECTED) {
    digitalWrite(kLEDPin, LOW);
    Serial.printf("Error %d connecting to WiFi\n", err);
    return err;
  }
  digitalWrite(kLEDPin, HIGH);
  err = Network::Fetch();
  DisconnectWiFi();
  DrawStatus();
  Serial.println("Successfully refreshed status");
  digitalWrite(kLEDPin, LOW);
  return 0;
}

void loop() {
  while (true) {
    FetchStatus();
    // Sleep one hour.
    delay(3600 * 1000);
  }
}
