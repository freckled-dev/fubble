#ifndef UUID_0589BF64_7763_43BC_9EA6_D3FBA6370DA2
#define UUID_0589BF64_7763_43BC_9EA6_D3FBA6370DA2

#include "fubble/rtc/data_channel_ptr.hpp"
#include "track_adder.hpp"
#include <boost/signals2/signal.hpp>

namespace client {
class add_data_channel : public track_adder {
public:
  ~add_data_channel();
  void add_to_connection(rtc::connection &connection) override;
  void remove_from_connection(rtc::connection &connection) override;
  boost::signals2::signal<void(rtc::data_channel_ptr)> on_added;
};
} // namespace client

#endif
