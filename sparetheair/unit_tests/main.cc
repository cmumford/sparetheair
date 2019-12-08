#include <fstream>
#include <streambuf>

#include "display.h"
#include "gtest/gtest.h"
#include "network.h"

namespace {

using spare_the_air::AQICategory;
using spare_the_air::Display;
using spare_the_air::Network;
using spare_the_air::RegionValues;
using spare_the_air::Status;

String ReadFile(const std::string& fname) {
  std::ifstream t(fname);
  std::string str((std::istreambuf_iterator<char>(t)),
                  std::istreambuf_iterator<char>());
  return String(str.c_str());
}

// ArduinoUnit can't handle enumerations, so cast to integer.
int CatToInt(AQICategory cat) {
  return static_cast<int>(cat);
}

TEST(Network, dayOfWeek) {
  Network::Reset();
  EXPECT_EQ(Network::ExtractDayOfWeek("BAAQMD Air Quality Forecast for Friday"),
            String("Friday"));
  EXPECT_EQ(Network::ExtractDayOfWeek("Saturday, November 23, 2019"),
            String("Saturday"));
  EXPECT_EQ(Network::ExtractDayOfWeek("Invalid date."), String());
  EXPECT_EQ(Network::ExtractDayOfWeek(""), String());
  EXPECT_EQ(Network::ExtractDayOfWeek("BAAQMD Air Quality Forecast for "),
            String());
}

TEST(Network, parseAQIName) {
  EXPECT_EQ(CatToInt(Network::ParseAQIName("Good")),
            CatToInt(AQICategory::Good));
  EXPECT_EQ(CatToInt(Network::ParseAQIName("Moderate")),
            CatToInt(AQICategory::Moderate));
  EXPECT_EQ(CatToInt(Network::ParseAQIName("Unhealthy for Sensitive Groups")),
            CatToInt(AQICategory::UnhealthyForSensitiveGroups));
  EXPECT_EQ(CatToInt(Network::ParseAQIName("Unhealthy")),
            CatToInt(AQICategory::Unhealthy));
  EXPECT_EQ(CatToInt(Network::ParseAQIName("Very Unhealthy")),
            CatToInt(AQICategory::VeryUnhealthy));
  EXPECT_EQ(CatToInt(Network::ParseAQIName("Hazardous")),
            CatToInt(AQICategory::Hazardous));

  EXPECT_EQ(CatToInt(Network::ParseAQIName("InvalidText")),
            CatToInt(AQICategory::None));
  EXPECT_EQ(CatToInt(Network::ParseAQIName("")), CatToInt(AQICategory::None));
}

TEST(Network, parseAQICategoryValue) {
  EXPECT_EQ(CatToInt(Network::AQIValueToCategory(0)),
            CatToInt(AQICategory::Good));
  EXPECT_EQ(CatToInt(Network::AQIValueToCategory(10)),
            CatToInt(AQICategory::Good));
  EXPECT_EQ(CatToInt(Network::AQIValueToCategory(51)),
            CatToInt(AQICategory::Moderate));
  EXPECT_EQ(CatToInt(Network::AQIValueToCategory(100)),
            CatToInt(AQICategory::Moderate));
  EXPECT_EQ(CatToInt(Network::AQIValueToCategory(101)),
            CatToInt(AQICategory::UnhealthyForSensitiveGroups));
  EXPECT_EQ(CatToInt(Network::AQIValueToCategory(150)),
            CatToInt(AQICategory::UnhealthyForSensitiveGroups));
  EXPECT_EQ(CatToInt(Network::AQIValueToCategory(151)),
            CatToInt(AQICategory::Unhealthy));
  EXPECT_EQ(CatToInt(Network::AQIValueToCategory(200)),
            CatToInt(AQICategory::Unhealthy));
  EXPECT_EQ(CatToInt(Network::AQIValueToCategory(201)),
            CatToInt(AQICategory::VeryUnhealthy));
  EXPECT_EQ(CatToInt(Network::AQIValueToCategory(300)),
            CatToInt(AQICategory::VeryUnhealthy));
  EXPECT_EQ(CatToInt(Network::AQIValueToCategory(301)),
            CatToInt(AQICategory::Hazardous));
  EXPECT_EQ(CatToInt(Network::AQIValueToCategory(600)),
            CatToInt(AQICategory::Hazardous));

  EXPECT_EQ(CatToInt(Network::AQIValueToCategory(-1)),
            CatToInt(AQICategory::None));
}

TEST(Network, aqiCategoryAbbrev) {
  EXPECT_EQ(Network::AQICategoryAbbrev(AQICategory::Good), String("G"));
  EXPECT_EQ(Network::AQICategoryAbbrev(AQICategory::Moderate), String("M"));
  EXPECT_EQ(
      Network::AQICategoryAbbrev(AQICategory::UnhealthyForSensitiveGroups),
      String("USG"));
  EXPECT_EQ(Network::AQICategoryAbbrev(AQICategory::Unhealthy), String("U"));
  EXPECT_EQ(Network::AQICategoryAbbrev(AQICategory::VeryUnhealthy),
            String("VU"));
  EXPECT_EQ(Network::AQICategoryAbbrev(AQICategory::Hazardous), String("H"));
  EXPECT_EQ(Network::AQICategoryAbbrev(AQICategory::None), String("?"));
}

TEST(Network, regionValues) {
  Network::Reset();

  const String region_data = ReadFile("../test_data/region_data.txt");
  RegionValues values =
      Network::ExtractRegionValues(region_data, "Eastern District");
  EXPECT_EQ(values.name, String("Eastern District"));
  EXPECT_EQ(values.aqi, String("53"));
  EXPECT_EQ(values.pollutant, String("PM2.5"));

  values = Network::ExtractRegionValues(region_data, "Santa Clara Valley");
  EXPECT_EQ(values.name, String("Santa Clara Valley"));
  EXPECT_EQ(values.aqi, String("55"));
  EXPECT_EQ(values.pollutant, String("PM2.6"));

  values = Network::ExtractRegionValues(region_data, "InvalidName");
  EXPECT_EQ(values.name, String());
  EXPECT_EQ(values.aqi, String());
  EXPECT_EQ(values.pollutant, String());
}

TEST(Network, parseAlert) {
  Network::Reset();
  Network::ParseAlert(ReadFile("../test_data/today.xml"));

  const Status& today = Network::AlertStatus();
  EXPECT_EQ(today.alert_status, String("Alert In Effect"));
  EXPECT_EQ(today.date_full, String("Saturday, November 23, 2019"));
  EXPECT_EQ(today.day_of_week, String("Saturday"));
  // The following values only come from the forecast.
  EXPECT_EQ(today.aqi_val, -1);
  EXPECT_EQ(CatToInt(today.aqi_category), CatToInt(AQICategory::None));
  EXPECT_EQ(today.pollutant, String());
}

TEST(Network, parseForecast) {
  Network::Reset();
  Network::ParseForecast(ReadFile("../test_data/forecast.xml"));

  const Status& first = Network::forecast(0);
  // First two only come from the alert.
  EXPECT_EQ(first.alert_status, String());
  EXPECT_EQ(first.date_full, String("BAAQMD Air Quality Forecast for Friday"));
  EXPECT_EQ(first.day_of_week, String("Friday"));
  EXPECT_EQ(first.aqi_val, 55);
  EXPECT_EQ(CatToInt(first.aqi_category), CatToInt(AQICategory::Moderate));
  EXPECT_EQ(first.pollutant, String("PM2.5"));

  const Status& second = Network::forecast(1);
  EXPECT_EQ(second.alert_status, String());
  EXPECT_EQ(second.date_full,
            String("BAAQMD Air Quality Forecast for Saturday"));
  EXPECT_EQ(second.day_of_week, String("Saturday"));
  EXPECT_EQ(second.aqi_val, 63);
  EXPECT_EQ(CatToInt(second.aqi_category), CatToInt(AQICategory::Moderate));
  EXPECT_EQ(second.pollutant, String("PM2.5"));
}

TEST(Network, fullFetch) {
  Network::Reset();

  // Simulate doing a full forecast/alert fetch.
  Network::ParseAlert(ReadFile("../test_data/today.xml"));
  Network::ParseForecast(ReadFile("../test_data/forecast.xml"));
  Network::MergeAlert();

  const Status& today = Network::status(0);
  EXPECT_EQ(today.alert_status, String("Alert In Effect"));
  EXPECT_EQ(today.date_full, String("Saturday, November 23, 2019"));
  EXPECT_EQ(today.day_of_week, String("Saturday"));
  // The following values only come from the forecast.
  EXPECT_EQ(today.aqi_val, 63);
  EXPECT_EQ(CatToInt(today.aqi_category), CatToInt(AQICategory::Moderate));
  EXPECT_EQ(today.pollutant, String("PM2.5"));
}

TEST(Network, failedFetch) {
  Network::Reset();
  const Status& today = Network::status(0);
  EXPECT_EQ(today.alert_status, String());
  EXPECT_EQ(today.date_full, String());
  EXPECT_EQ(today.day_of_week, String());
  EXPECT_EQ(today.aqi_val, -1);
  EXPECT_EQ(CatToInt(today.aqi_category), CatToInt(AQICategory::None));
  EXPECT_EQ(today.pollutant, String());
}
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
