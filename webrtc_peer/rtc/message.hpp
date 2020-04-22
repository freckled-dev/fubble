#ifndef RTC_MESSAGE_HPP
#define RTC_MESSAGE_HPP

#include <cstddef>
#include <string>
#include <vector>

namespace rtc {
struct message {
  std::vector<std::uint8_t> data;
  bool binary;
  std::string to_string() const;
};
} // namespace rtc

#endif
