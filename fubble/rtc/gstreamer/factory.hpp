#ifndef UUID_A30958E9_D209_4E0E_B0B0_B2D9E06F18B5
#define UUID_A30958E9_D209_4E0E_B0B0_B2D9E06F18B5

#include <boost/thread/executor.hpp>
#include <fubble/rtc/factory.hpp>

namespace rtc::gstreamer {
class factory : public rtc::factory {
public:
  factory(std::shared_ptr<boost::executor> executor);

  std::unique_ptr<rtc::connection> create_connection() override;
  std::unique_ptr<audio_track>
  create_audio_track(audio_source &source) override;
  std::unique_ptr<rtc::video_track>
  create_video_track(const std::shared_ptr<rtc::video_source> &source) override;
  std::shared_ptr<rtc::audio_devices> get_audio_devices() override;

protected:
  std::shared_ptr<boost::executor> executor;
};
} // namespace rtc::gstreamer

#endif
