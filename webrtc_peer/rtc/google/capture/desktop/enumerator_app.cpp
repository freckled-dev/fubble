#include "logging/initialser.hpp"
#include "rtc/google/capture/desktop/capturer.hpp"
#include "rtc/google/capture/desktop/enumerator.hpp"
#include <fmt/format.h>

int main(int, char *[]) {
  logging::add_console_log(logging::severity::trace);
  logging::logger logger{"main"};
  auto enumerator = rtc::google::capture::desktop::enumerator::create();
  enumerator->enumerate();
  BOOST_LOG_SEV(logger, logging::severity::info) << "windows:";
  for (auto print : enumerator->get_windows())
    BOOST_LOG_SEV(logger, logging::severity::info)
        << fmt::format("id:'{}', title:'{}'", print.id, print.title);
  BOOST_LOG_SEV(logger, logging::severity::info) << "screens:";
  for (auto print : enumerator->get_screens())
    BOOST_LOG_SEV(logger, logging::severity::info)
        << fmt::format("id:'{}', title:'{}'", print.id, print.title);
  BOOST_ASSERT(!enumerator->get_windows().empty());
  BOOST_ASSERT(!enumerator->get_screens().empty());
  std::intptr_t window_id = enumerator->get_windows().front().id;
  auto capturer =
      rtc::google::capture::desktop::capturer::create_window(window_id, false);
  for (int counter{}; counter < 4; ++counter)
    capturer->capture().get();
  return 0;
}
