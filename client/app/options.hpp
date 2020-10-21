#ifndef CLIENT_OPTIONS_HPP
#define CLIENT_OPTIONS_HPP

#include <boost/optional.hpp>
#include <string>

struct config {
  struct video {
    bool send{false};
  };
  video video_;
  struct signalling {
    std::string host{"localhost"};
    std::string service{"http"};
    std::string id{"test"}; // TODO remove this flag. join a room!
  };
  signalling signalling_;
#if 0
  struct matrix {
    std::string host{"localhost"};
    std::string service{"8008"};
  };
  matrix matrix_;
  struct temporary_room {
    std::string host{"localhost"};
    std::string service{"8009"};
  };
  temporary_room temporary_room_;
#endif
};

class options {
public:
  boost::optional<config> operator()(int argc, char *argv[]);
};

#endif
