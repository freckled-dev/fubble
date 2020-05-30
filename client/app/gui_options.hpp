#ifndef UUID_CD44B89A_7977_4C7E_9EB2_983FCDCA2681
#define UUID_CD44B89A_7977_4C7E_9EB2_983FCDCA2681

#include "logging/logger.hpp"
#include <optional>
#include <string>

struct gui_config {
  struct general {
    std::string host{"fubble.io"};
    std::string service{"https"};
    bool use_ipv6{false};
    bool use_ssl{true};
    logging::severity log_severity{logging::severity::trace};
  };
  general general_;
};

class gui_options {
public:
  std::optional<gui_config> parse(int argc, char *argv[]);
};

#endif
