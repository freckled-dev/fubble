#ifndef PORT_HPP
#define PORT_HPP

#include <cstdint>

struct port {
  std::uint16_t value;
  constexpr operator int() const { return value; }
};

#endif  // PORT_HPP
