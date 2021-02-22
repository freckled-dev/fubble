#include "factory.hpp"
#include <fubble/rtc/gstreamer/connection.hpp>

using namespace rtc::gstreamer;

factory::factory(std::shared_ptr<boost::executor> executor)
    : executor{executor} {}

std::unique_ptr<rtc::connection> factory::create_connection() {
  return std::make_unique<connection>(*executor);
}
