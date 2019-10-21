#include "logging/logger.hpp"
#include <boost/log/keywords/auto_flush.hpp>
#include <boost/log/keywords/format.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <gtest/gtest.h>
extern "C" {
#include <gst/gst.h>
}

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);

  boost::log::add_common_attributes();
  boost::log::add_console_log(
      std::cout, boost::log::keywords::auto_flush = true,
      boost::log::keywords::format = "%TimeStamp% %Severity% %Message%");

  GError *error{};
  if (!gst_init_check(&argc, &argv, &error)) {
    logging::logger logger;
    BOOST_LOG_SEV(logger, logging::severity::error)
        << "could not initialise gstreamer, message:" << error->message;
    return 1;
  }
  auto result = RUN_ALL_TESTS();
  gst_deinit();
  return result;
}
