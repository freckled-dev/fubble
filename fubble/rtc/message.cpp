#include "message.hpp"

using namespace rtc;

std::string message::to_string() const {
  auto data_casted = reinterpret_cast<const char *>(data.data());
  return std::string(data_casted, data.size());
}

