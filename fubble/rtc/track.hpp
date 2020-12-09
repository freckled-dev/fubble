#ifndef RTC_TRACK_HPP
#define RTC_TRACK_HPP

namespace rtc {
class track {
public:
  virtual ~track() = default;
  virtual void set_enabled(bool) = 0;
};
} // namespace rtc

#endif
