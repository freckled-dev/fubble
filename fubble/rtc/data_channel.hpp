#ifndef RTC_DATA_CHANNEL_HPP
#define RTC_DATA_CHANNEL_HPP

#include "message.hpp"
#include <sigslot/signal.hpp>

namespace rtc {
class data_channel {
public:
  virtual ~data_channel() = default;
  virtual void close() = 0;
  sigslot::signal<> on_opened;
  sigslot::signal<const message &> on_message;
  void send(const std::string &message);
  void send(const std::vector<uint8_t> &message);
  virtual void send(const message &message_) = 0;
};
} // namespace rtc

#endif
