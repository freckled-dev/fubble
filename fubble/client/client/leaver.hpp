#ifndef UUID_93601485_1C7B_4CDE_9303_1CB02F7FBA15
#define UUID_93601485_1C7B_4CDE_9303_1CB02F7FBA15

#include "fubble/client/logger.hpp"
#include <boost/signals2/signal.hpp>
#include <boost/thread/executors/inline_executor.hpp>
#include <boost/thread/future.hpp>

namespace client {
class rooms;
class leaver {
public:
  leaver(rooms &rooms_);

  boost::future<void> leave();
  boost::signals2::signal<void()> on_about_to_leave;

protected:
  void on_left(boost::future<void> &result);

  client::logger logger{"leaver"};
  boost::inline_executor executor;
  rooms &rooms_;
};
} // namespace client

#endif
