#include "data_channel.hpp"
#include <fmt/format.h>

using namespace rtc::google;

data_channel::data_channel(
    ::rtc::scoped_refptr<webrtc::DataChannelInterface> native_)
    : native(native_) {
  native->RegisterObserver(this);
}

data_channel::~data_channel() = default;

void data_channel::OnStateChange() {
  const webrtc::DataChannelInterface::DataState state = native->state();
  BOOST_LOG_SEV(logger, logging::severity::info)
      << "OnStateChange, state:" << state;
}

void data_channel::OnMessage(const webrtc::DataBuffer &buffer) {
  BOOST_LOG_SEV(logger, logging::severity::info) << fmt::format(
      "OnMessage, size:'{}', binary:{}", buffer.size(), buffer.binary);
}

void data_channel::OnBufferedAmountChange(uint64_t sent_data_size) {
  BOOST_LOG_SEV(logger, logging::severity::info)
      << fmt::format("OnBufferedAmountChange, size:'{}'", sent_data_size);
}

