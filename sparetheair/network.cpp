// Copyright 2019 Christopher Mumford
// This code is licensed under MIT license (see LICENSE for details)

#include <HTTPClient.h>

#include "network.h"
#include "parser.h"
#include "status.h"

namespace spare_the_air {

namespace {

struct HttpFetchResult {
  int httpCode;
  String response;
};

constexpr const char kAlertUrl[] = "https://www.baaqmd.gov/Feeds/AlertRSS.aspx";
constexpr const char kForecastUrl[] =
    "https://www.baaqmd.gov/Feeds/AirForecastRSS.aspx";

HttpFetchResult DoHTTPGet(const char* url) {
  HttpFetchResult result;
  HTTPClient client;
  client.begin(url);
  result.httpCode = client.GET();
  if (result.httpCode == HTTP_CODE_OK)
    result.response = client.getString();
  return result;
}

}  // namespace

int Network::Fetch() {
  Parser::Reset();
  int err = FetchAlert();
  if (err == HTTP_CODE_OK) {
    err = FetchForecast();
    Parser::MergeAlert();
  } else {
    FetchForecast();
  }

  return err;
}

int Network::FetchAlert() {
  HttpFetchResult result = DoHTTPGet(kAlertUrl);
  if (result.httpCode != HTTP_CODE_OK)
    return result.httpCode;

  Parser::ParseAlert(result.response);
  return HTTP_CODE_OK;
}

int Network::FetchForecast() {
  HttpFetchResult result = DoHTTPGet(kForecastUrl);
  if (result.httpCode != HTTP_CODE_OK)
    return result.httpCode;

  Parser::ParseForecast(result.response);
  return HTTP_CODE_OK;
}

const Status& Network::status(int idx) const {
  return Parser::status(idx);
}

}  // namespace spare_the_air
