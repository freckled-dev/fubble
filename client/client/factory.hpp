#ifndef UUID_7A298A6E_2EBE_478E_B1F6_445F9579CE7B
#define UUID_7A298A6E_2EBE_478E_B1F6_445F9579CE7B

#include <boost/asio/io_context.hpp>

namespace rtc::google {
class audio_track;
}

namespace client {
class factory {
public:
  factory(boost::asio::io_context &context);

protected:
  boost::asio::io_context &context;
};
} // namespace client

#endif
