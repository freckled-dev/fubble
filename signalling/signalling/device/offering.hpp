#ifndef SIGNALLING_DEVICE_OFFERING_HPP
#define SIGNALLING_DEVICE_OFFERING_HPP

#include "answering_ptr.hpp"
#include "signalling/connection_ptr.hpp"
#include <boost/signals2/signal.hpp>

namespace signalling::device {
class offering {
public:
  offering(connection_ptr connection_);
  void set_partner(const answering_ptr &partner);
  boost::signals2::signal<void()> on_closed;
};
} // namespace signalling::device

#endif
