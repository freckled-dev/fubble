#ifndef CLIENT_OPTIONS_HPP
#define CLIENT_OPTIONS_HPP

#include <optional>
#include <string>

struct config {
  struct video {
    bool send{false};
  };
  video video_;
  struct signalling {
    std::string host{"localhost"};
    std::string service{"http"};
    std::string id{"test"};
  };
  signalling signalling_;
};

class options {
public:
  std::optional<config> operator()(int argc, char *argv[]);
};

#endif