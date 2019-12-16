// Copyright 2019 Christopher Mumford
// This code is licensed under MIT license (see LICENSE for details)

#ifndef SPARETHEAIR_NETWORK_H_
#define SPARETHEAIR_NETWORK_H_

#include <Arduino.h>

namespace spare_the_air {

struct Status;

// The maximum number of status days returned by Network::status().
const int kNumStatusDays = 4;

class Network {
 public:
  // Fetch the alert/forecasts from the network. Returns 0 if successful.
  int Fetch();

  // Index 0 is always today, and index 1 is tomorrow, etc.
  // idx is in the range [0, kNumStatusDays).
  const Status& status(int idx) const;

 private:
  // Fetch and parse the alert from the network. Returns 0 upon success.
  int FetchAlert();
  // Fetch and parse the forecasts from the network. Returns 0 upon success.
  int FetchForecast();
};

}  // namespace spare_the_air

#endif  // SPARETHEAIR_NETWORK_H_
