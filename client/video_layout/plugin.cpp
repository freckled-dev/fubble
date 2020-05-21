#include "plugin.hpp"
#include "video_layout.hpp"

void plugin::registerTypes(const char *uri) {
  qmlRegisterType<video_layout>("io.fubble", 1, 0, "VideoLayout");
}
