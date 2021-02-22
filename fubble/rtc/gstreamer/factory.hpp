#ifndef UUID_A30958E9_D209_4E0E_B0B0_B2D9E06F18B5
#define UUID_A30958E9_D209_4E0E_B0B0_B2D9E06F18B5

#include <boost/thread/executor.hpp>
#include <fubble/rtc/factory.hpp>

namespace rtc::gstreamer {
class factory : public rtc::factory {
public:
  factory(std::shared_ptr<boost::executor> executor);

  std::unique_ptr<rtc::connection> create_connection() override;

protected:
  std::shared_ptr<boost::executor> executor;
};
} // namespace rtc::gstreamer

#endif
