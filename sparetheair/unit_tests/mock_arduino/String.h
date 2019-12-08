#ifndef MOCK_ARDUINO_STRING_H
#define MOCK_ARDUINO_STRING_H

#include <string>

class String : public std::string {
  public:
    String() = default;
    String(const char* val) : std::string(val) {}
    String(int val) {}

    String substring(int idx, int len = -1) const {
      return String();
    }

    int indexOf(const String& substr, int startIdx = -1) const {
      return -1;
    }

    int indexOf(const char* substr, int startIdx = -1) const {
      return -1;
    }
};

String operator+(const String& lhs, const String& rhs) {
  return String();
}

class __FlashStringHelper {
};

#endif  // MOCK_ARDUINO_STRING_H
