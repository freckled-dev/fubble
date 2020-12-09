#include "data_channel.hpp"

using namespace rtc;

void data_channel::send(const std::string &message_) {
  message casted;
  casted.binary = false;
  auto data_ptr = reinterpret_cast<const std::uint8_t *>(message_.data());
  casted.data = std::vector<std::uint8_t>(data_ptr, data_ptr + message_.size());
  send(casted);
}
void data_channel::send(const std::vector<std::uint8_t> &message_) {
  message casted;
  casted.binary = true;
  auto data_ptr = reinterpret_cast<const std::uint8_t *>(message_.data());
  casted.data = std::vector<uint8_t>(data_ptr, data_ptr + message_.size());
  send(casted);
}
