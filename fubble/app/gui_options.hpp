#ifndef UUID_CD44B89A_7977_4C7E_9EB2_983FCDCA2681
#define UUID_CD44B89A_7977_4C7E_9EB2_983FCDCA2681

#include "core_module.hpp"
#include <boost/optional.hpp>
#include <fubble/utils/logging/logger.hpp>
#include <string>

struct gui_config {
  struct general {
    std::string host{"fubble.io"};
    std::string service{"https"};
    bool use_ipv6{true};
    bool use_ssl{true};
    logging::severity log_severity{logging::severity::debug};
    bool video_support{true};
    bool desktop_support{true};
    bool use_crash_catcher{true};
    bool log_webrtc{false};
  };
  general general_;
  client::core_module::config client_core;
};

class gui_options {
public:
  boost::optional<gui_config> parse(int argc, char *argv[]);
};

#endif
