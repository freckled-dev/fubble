#include "data_channel.hpp"
#include <fmt/format.h>

using namespace rtc::google;

namespace {
std::string to_string(const webrtc::DataChannelInterface::DataState print) {
  using type = webrtc::DataChannelInterface::DataState;
  switch (print) {
  case type::kClosed:
    return "closed";
  case type::kClosing:
    return "closing";
  case type::kConnecting:
    return "connecting";
  case type::kOpen:
    return "open";
  }
  BOOST_ASSERT(false);
  return "";
}
} // namespace

data_channel::data_channel(
    ::rtc::scoped_refptr<webrtc::DataChannelInterface> native_)
    : native(native_) {
  native->RegisterObserver(this);
}

data_channel::~data_channel() { native->UnregisterObserver(); }

void data_channel::close() { native->Close(); }

void data_channel::send(const message &message_) {
  const char *data_ptr = reinterpret_cast<const char *>(message_.data.data());
  rtc::CopyOnWriteBuffer buffer{data_ptr, message_.data.size()};
  const webrtc::DataBuffer wrapped{buffer, message_.binary};
  [[maybe_unused]] auto success = native->Send(wrapped);
  BOOST_ASSERT(success);
}

void data_channel::OnStateChange() {
  const webrtc::DataChannelInterface::DataState state = native->state();
  BOOST_LOG_SEV(logger, logging::severity::info)
      << "data channel OnStateChange, state:" << to_string(state);
  if (state == webrtc::DataChannelInterface::DataState::kOpen)
    on_opened();
}

void data_channel::OnMessage(const webrtc::DataBuffer &buffer) {
  BOOST_LOG_SEV(logger, logging::severity::info) << fmt::format(
      "OnMessage, size:'{}', binary:{}", buffer.size(), buffer.binary);
  message result;
  result.binary = buffer.binary;
  const std::uint8_t *data_ptr =
      reinterpret_cast<const std::uint8_t *>(buffer.data.cdata());
  const auto data_size = static_cast<std::size_t>(buffer.size());
  result.data = std::vector<std::uint8_t>(data_ptr, data_ptr + data_size);
  on_message(result);
}

void data_channel::OnBufferedAmountChange(uint64_t sent_data_size) {
  BOOST_LOG_SEV(logger, logging::severity::info)
      << fmt::format("OnBufferedAmountChange, size:'{}'", sent_data_size);
  // TODO ensure that not more than 16MB are getting buffered!
}
