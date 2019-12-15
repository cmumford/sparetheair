#include "Arduino_String.h"

// static
String operator+(const String& lhs, const String& rhs) {
  std::string n = lhs;
  n += rhs;
  return String(n.c_str());
}

// static
void PrintTo(const String& string, std::ostream* os) {
  *os << '"' << string << '"';
}

String::String(const char* val) : std::string(val) {}

String::String(int val) : std::string(std::to_string(val)) {}

String String::substring(int from) const {
  return String(substr(from).c_str());
}

String String::substring(int from, int to) const {
  return String(substr(from, to - from).c_str());
}

int String::indexOf(const String& substr) const {
  return indexOf(substr.c_str());
}

int String::indexOf(const char* substr) const {
  size_t pos = find(substr);
  if (pos == npos)
    return -1;
  return pos;
}

int String::indexOf(const String& substr, int from) const {
  return indexOf(substr.c_str(), from);
}

int String::indexOf(const char* substr, int from) const {
  size_t pos = find(substr, from);
  if (pos == npos)
    return -1;
  return pos;
}
