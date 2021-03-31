#ifndef UUID_0387C1F6_A591_404D_A2DF_3EB446E6A031
#define UUID_0387C1F6_A591_404D_A2DF_3EB446E6A031

#include <boost/signals2/signal.hpp>
#include <fubble/v4l2_hw_h264/config.hpp>
#include <memory>

namespace fubble {
namespace v4l2_hw_h264 {

class reader {
public:
  virtual ~reader() = default;
  virtual void initialise(unsigned int start_bitrate) = 0;
  virtual void set_bitrate(int bitrate) = 0;
  virtual void trigger_i_frame() = 0;
  virtual void mainloop() = 0;
  boost::signals2::signal<void(const void *data, int size)> on_data;

  static std::unique_ptr<reader> create(config config_);
  static std::unique_ptr<reader> create_shared(std::shared_ptr<reader>);
};

} // namespace v4l2_hw_h264
} // namespace fubble

#endif
