#ifndef MOCK_ARDUINO_HTTPCLIENT_H
#define MOCK_ARDUINO_HTTPCLIENT_H

#include "Arduino_String.h"

#define HTTP_CODE_OK 200

class HTTPClient {
 public:
  HTTPClient();

  void begin(String url);
  int GET();
  String getString();
};

#endif  // MOCK_ARDUINO_HTTPCLIENT_H
