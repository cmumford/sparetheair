#ifndef MOCK_ARDUINO_PRINT_H
#define MOCK_ARDUINO_PRINT_H

#include "String.h"

class Print {
 public:
  Print() = default;
  void print(const String& str);
};

#endif  // MOCK_ARDUINO_PRINT_H
