#ifndef RTC_GOOGLE_CONNECTION_HPP
#define RTC_GOOGLE_CONNECTION_HPP

#include "logging/logger.hpp"
#include "rtc/connection.hpp"

namespace rtc::google {
class connection : public rtc::connection {
public:
  ~connection() override;
};
} // namespace rtc::google

#endif
