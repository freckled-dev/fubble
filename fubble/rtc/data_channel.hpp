#ifndef RTC_DATA_CHANNEL_HPP
#define RTC_DATA_CHANNEL_HPP

#include "message.hpp"
#include <boost/signals2/signal.hpp>

namespace rtc {
class data_channel {
public:
  virtual ~data_channel() = default;
  virtual void close() = 0;
  boost::signals2::signal<void()> on_opened;
  boost::signals2::signal<void(const message &)> on_message;
  void send(const std::string &message);
  void send(const std::vector<uint8_t> &message);
  virtual void send(const message &message_) = 0;
};
} // namespace rtc

#endif
