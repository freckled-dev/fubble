#ifndef RTC_CONNECTION_HPP
#define RTC_CONNECTION_HPP

#include "ice_candidate.hpp"
#include "session_description.hpp"
#include "track_ptr.hpp"
#include <boost/signals2/signal.hpp>
#include <boost/thread/future.hpp>

namespace rtc {
class track;
using track_ptr = std::shared_ptr<track>;

class connection {
public:
  virtual ~connection();
  virtual boost::future<session_description> create_offer() = 0;
  virtual boost::future<void>
  set_local_description(const session_description &) = 0;
  virtual boost::future<void>
  set_remote_description(const session_description &) = 0;
  virtual void add_track(track_ptr) = 0;
  boost::signals2::signal<void()> on_negotiation_needed;
  boost::signals2::signal<void(track_ptr)> on_track;
  boost::signals2::signal<void(ice_candidate)> on_ice_candidate;
};
} // namespace rtc

#endif
