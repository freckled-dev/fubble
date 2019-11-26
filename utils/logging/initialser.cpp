#include "initialser.hpp"
#include <boost/log/keywords/auto_flush.hpp>
#include <boost/log/keywords/format.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <iostream>

void logging::add_console_log() {
  boost::log::add_common_attributes();
  boost::log::add_console_log(
      std::cout, boost::log::keywords::auto_flush = true,
      boost::log::keywords::format = "%TimeStamp% %Severity% %Message%");
}
