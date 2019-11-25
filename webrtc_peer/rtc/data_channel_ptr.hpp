#ifndef RTC_DATA_CHANNEL_PTR_HPP
#define RTC_DATA_CHANNEL_PTR_HPP

#include <memory>

namespace rtc {
class data_channel;
using data_channel_ptr = std::shared_ptr<data_channel>;
} // namespace rtc

#endif
