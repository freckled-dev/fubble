#ifndef UUID_D8146110_5612_437B_A8CF_F644AC1A9BEE
#define UUID_D8146110_5612_437B_A8CF_F644AC1A9BEE

#include "fubble/utils/logging/logger.hpp"

namespace client::ui {
class log_qt_to_logging {
public:
  log_qt_to_logging();
  ~log_qt_to_logging();

protected:
  logging::logger logger;
};
} // namespace client::ui

#endif
