// Copyright 2019 Christopher Mumford
// This code is licensed under MIT license (see LICENSE for details)

#include <HTTPClient.h>
#include <TinyXML.h>

#include "network.h"
#include "status.h"

namespace spare_the_air {

void Status::Reset() {
  alert_status = "";
  date_full = "";
  day_of_week = "";
  aqi_val = -1;
  aqi_category = AQICategory::None;
  pollutant = "";
}

}  // namespace spare_the_air
