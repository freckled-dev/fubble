#include "options.hpp"
#include "fubble/utils/options_adder.hpp"
#include <iostream>

using namespace fubble::client;

namespace bpo = boost::program_options;

boost::program_options::options_description
options::create(::client::log_module::config &config) {
  boost::program_options::options_description result{"logging"};
  utils::option_adder adder{result};
  adder.add("log-severity", config.severity, "severity");
  adder.add("log-webrtc", config.webrtc, "log webrtc (might be quite verbose)");
  return result;
}

boost::program_options::options_description
options::create(::client::core_module::config &config) {
  boost::program_options::options_description result{"core"};
  utils::option_adder adder{result};
  adder.add("audio-layer", config.rtc_.audio_.layer_,
            "choose the audio-layer to use. eg. linux_alsa or linux_pulse");
  return result;
}

boost::program_options::options_description
options::create(v4l2_hw_h264::config &config) {
  boost::program_options::options_description result{"v4l2_hw_h264"};
  utils::option_adder adder{result};
  adder.add("v4l2_hw_h264-width", config.width, "video width");
  adder.add("v4l2_hw_h264-height", config.height, "video height");
  adder.add("v4l2_hw_h264-framerate", config.framerate, "video framerate");
  adder.add("v4l2_hw_h264-device", config.path, "video device to use");
  return result;
}
