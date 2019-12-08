#ifndef MOCK_ARDUINO_ARDUINO_H
#define MOCK_ARDUINO_ARDUINO_H

#include <string.h>

#include <cstdint>

#include "String.h"

#define PROGMEM

using boolean = bool;

inline int pgm_read_word(const void* addr) {
  int word;
  memcpy(&word, addr, sizeof(word));
  return word;
}

inline uint8_t pgm_read_byte(const void* addr) {
  return *static_cast<const uint8_t*>(addr);
}

#endif  // MOCK_ARDUINO_ARDUINO_H
