#ifndef UUID_CA059DA4_9A1A_4BE7_BED7_840D36B60BED
#define UUID_CA059DA4_9A1A_4BE7_BED7_840D36B60BED

#include <boost/asio/io_context.hpp>
#include <fruit/fruit_forward_decls.h>

namespace client {
class crash_catcher {
public:
  virtual ~crash_catcher() = default;

  static fruit::Component<fruit::Required<boost::asio::io_context>,
                          crash_catcher>
  create();
};
} // namespace client

#endif
