#ifndef RTC_GOOGLE_DATA_CHANNEL_HPP
#define RTC_GOOGLE_DATA_CHANNEL_HPP

#include "fubble/rtc/data_channel.hpp"
#include "fubble/rtc/logger.hpp"
#include <api/data_channel_interface.h>

namespace rtc {
namespace google {
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
  class logger logger {
    "data_channel"
  };
  ::rtc::scoped_refptr<webrtc::DataChannelInterface> native;
};
} // namespace google
} // namespace rtc

#endif
