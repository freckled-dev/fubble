#ifndef UUID_540C3C00_41CE_4548_8C0E_8191DC904C77
#define UUID_540C3C00_41CE_4548_8C0E_8191DC904C77

#include <boost/signals2/signal.hpp>
#include <fubble/rtc/video_frame.hpp>
#include <memory>

namespace rtc {
class video_source {
public:
  virtual ~video_source() = default;

  boost::signals2::signal<void(const rtc::video_frame &)> on_frame;
};

class video_source_noop : public video_source {
public:
};

using video_source_ptr = std::shared_ptr<video_source>;
} // namespace rtc

#endif
