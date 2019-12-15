#ifndef MOCK_ARDUINO_ARDUINO_STRING_H
#define MOCK_ARDUINO_ARDUINO_STRING_H

#include <string>
#include <ostream>

class String : public std::string {
 public:
  String() = default;
  String(const char* val);
  String(int val);

  String substring(int from) const;
  String substring(int from, int to) const;
  int indexOf(const String& substr) const;
  int indexOf(const char* substr) const;
  int indexOf(const String& substr, int from) const;
  int indexOf(const char* substr, int from) const;

  friend void PrintTo(const String& string, std::ostream* os);
};

String operator+(const String& lhs, const String& rhs);

class __FlashStringHelper {};

#endif  // MOCK_ARDUINO_ARDUINO_STRING_H
