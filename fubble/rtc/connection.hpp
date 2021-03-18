#ifndef RTC_CONNECTION_HPP
#define RTC_CONNECTION_HPP

#include <boost/signals2/signal.hpp>
#include <boost/thread/future.hpp>
#include <fubble/rtc/data_channel_ptr.hpp>
#include <fubble/rtc/ice_candidate.hpp>
#include <fubble/rtc/session_description.hpp>
#include <fubble/rtc/track_ptr.hpp>
#include <functional>

namespace rtc {
class connection {
public:
  virtual ~connection();
  struct offer_options {
    bool offer_to_receive_audio{true};
    bool offer_to_receive_video{true};
    bool voice_activity_detection{true};
  };
  virtual boost::future<session_description>
  create_offer(const offer_options &options) = 0;
  virtual boost::future<session_description> create_answer() = 0;
  virtual boost::future<void>
  set_local_description(const session_description &) = 0;
  virtual boost::future<void>
  set_remote_description(const session_description &) = 0;
  virtual void add_ice_candidate(const ice_candidate &candidate) = 0;
  virtual void add_track(track_ptr) = 0;
  virtual void remove_track(track_ptr) = 0;
  virtual rtc::data_channel_ptr create_data_channel() = 0;
  virtual void close() = 0;
  using stats_callback = std::function<void(std::string)>;
  virtual void get_stats(const stats_callback &callback) = 0;
  boost::signals2::signal<void()> on_negotiation_needed;
  boost::signals2::signal<void(track_ptr)> on_track_added;
  boost::signals2::signal<void(track_ptr)> on_track_removed;
  boost::signals2::signal<void(track_ptr)> on_video_track_added;
  boost::signals2::signal<void(track_ptr)> on_audio_track_added;
  boost::signals2::signal<void(data_channel_ptr)> on_data_channel;
  boost::signals2::signal<void(ice_candidate)> on_ice_candidate;
  boost::signals2::signal<void()> on_closed;
};
} // namespace rtc

#endif
