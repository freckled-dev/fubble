#ifndef CLIENT_OPTIONS_HPP
#define CLIENT_OPTIONS_HPP

#include <optional>
#include <string>

struct config {
  struct signalling {
    std::string host{"localhost"};
    std::string service{"http"};
    std::string id;
  };
  signalling signalling_;
};

class options {
public:
  std::optional<config> operator()(int argc, char *argv[]);
};

#endif
