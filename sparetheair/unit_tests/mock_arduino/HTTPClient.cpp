#include "HTTPClient.h"

HTTPClient::HTTPClient() = default;

void HTTPClient::begin(String url) {}

int HTTPClient::GET() {
  return HTTP_CODE_OK;
}

String HTTPClient::getString() {
  return String();
}
