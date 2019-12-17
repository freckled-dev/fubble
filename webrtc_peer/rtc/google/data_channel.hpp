#ifndef RTC_GOOGLE_DATA_CHANNEL_HPP
#define RTC_GOOGLE_DATA_CHANNEL_HPP

#include "logging/logger.hpp"
#include "rtc/data_channel.hpp"
#include <api/data_channel_interface.h>

namespace rtc::google {
class data_channel : protected ::webrtc::DataChannelObserver,
                     public ::rtc::data_channel {
public:
  data_channel(::rtc::scoped_refptr<webrtc::DataChannelInterface> native_);
  ~data_channel() override;

  void close() override;
  void send(const message &message_) override;

protected:
  void OnStateChange() override;
  void OnMessage(const webrtc::DataBuffer &buffer) override;
  void OnBufferedAmountChange(uint64_t sent_data_size) override;

private:
  logging::logger logger;
  ::rtc::scoped_refptr<webrtc::DataChannelInterface> native;
};
} // namespace rtc::google

#endif