#ifndef _SPARETHEAIR_H_
#define _SPARETHEAIR_H_

namespace sta {

// Status for a date. This may be the current status
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
  void ResetForTest();
};

struct RegionValues {
  String name;
  String aqi;
  String pollutant;
};

const int kNumForecastDays = 4;

class SpareTheAir {
 public:
  static int Fetch();

  SpareTheAir() = delete;
  ~SpareTheAir() = delete;

  static const Status& status(int idx);

 public:
  static String ExtractDayOfWeek(const String& date_full);
  static int FetchAlert();
  static int FetchForecast();
  static void ParseAlert(const String& xmlString);
  static void ParseForecast(const String& xmlString);
  static void ResetForTest();
};

}  // namespace sta

#endif  // _SPARETHEAIR_H_
