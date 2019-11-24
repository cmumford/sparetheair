// Copyright 2019 Christopher Mumford
// This code is licensed under MIT license (see LICENSE for details)
#include <iostream>
#include <string>
#include <vector>

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
  std::string alert_status;

  // Full date (if available) (e.g. "Sunday, November 17, 2019").
  // Will be empty for forecasts.
  std::string date_full;

  // Full day of week. i.e. "Sunday".
  std::string day_of_week;

  // The numerical AQI value in the range (0, 500). May not be available
  // for all items. If not available will be set to -1.
  int aqi_val;

  // One of ("Good", "Moderate", "Unhealthy for Sensitive Groups",
  //         "Unhealthy", "Very Unhealthy", "Hazardous").
  std::string aqi_name;

  // e.g. "PM2.5".
  std::string pollutant;

  bool AlertInEffect() const { return alert_status == "Alert In Effect"; }
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
    kEPaperSize.width * 1 / 3, kEPaperSize.width * 2 / 3,
};
const Rectangle kForecastBounds[3] = {
    {Point({0 * kForecastWidth, kTodayBounds.bottom()}),
     Point({kDividers[0] - 1, kEPaperBounds.bottom()})},
    {Point({kDividers[0], kTodayBounds.bottom()}),
     Point({kDividers[1] - 1, kEPaperBounds.bottom()})},
    {Point({kDividers[1], kTodayBounds.bottom()}),
     Point({kEPaperBounds.right(), kEPaperBounds.bottom()})},
};

// Read the status from the network and return an array of Status
// objects in chronological order - the first one being for today.
std::vector<Status> GetStatus() {
  std::vector<Status> status;

  return status;
}

}

using namespace std;

int main(int argc, const char* argv[]) {
  cout << "Hello world" << endl;
  return 0;
}
