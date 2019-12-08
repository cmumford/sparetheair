#ifndef MOCK_ARDUINO_HTTPCLIENT_H
#define MOCK_ARDUINO_HTTPCLIENT_H

#include "String.h"

#define HTTP_CODE_OK 200

class HTTPClient {
  public:
    HTTPClient();

    void begin(String url) {}
    int GET() {
      return HTTP_CODE_OK;
    }
    String getString() {
      return String();
    }
};

#endif  // MOCK_ARDUINO_HTTPCLIENT_H
