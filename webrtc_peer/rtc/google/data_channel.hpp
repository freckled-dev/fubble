#ifndef RTC_GOOGLE_DATA_CHANNEL_HPP
#define RTC_GOOGLE_DATA_CHANNEL_HPP

#include "logging/logger.hpp"
#include <api/data_channel_interface.h>
#include <boost/signals2/signal.hpp>

namespace rtc::google {
class data_channel : protected ::webrtc::DataChannelObserver {
public:
  data_channel(::rtc::scoped_refptr<webrtc::DataChannelInterface> native_);
  ~data_channel() override;

  boost::signals2::signal<void()> on_opened;

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
