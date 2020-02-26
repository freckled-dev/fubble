#include "initialser.hpp"
#include "logger.hpp"
#include <boost/log/expressions.hpp>
#include <boost/log/keywords/auto_flush.hpp>
#include <boost/log/keywords/format.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <iostream>

BOOST_LOG_ATTRIBUTE_KEYWORD(tag_attr, "Tag", std::string)
BOOST_LOG_ATTRIBUTE_KEYWORD(module_attr, "Module", std::string)

namespace {
auto date_time_formatter =
    boost::log::expressions::stream
    << boost::log::expressions::format_date_time<boost::posix_time::ptime>(
           "TimeStamp", "%Y-%m-%d %H:%M:%S.%f");
auto process_formatter =
    boost::log::expressions::stream << boost::log::expressions::attr<
        boost::log::attributes::current_process_id::value_type>("PID");
auto thread_formatter =
    boost::log::expressions::stream << boost::log::expressions::attr<
        boost::log::attributes::current_thread_id::value_type>("TID");
auto tag_formatter = boost::log::expressions::stream
                     << boost::log::expressions::attr<std::string>("Tag");
auto module_formatter = boost::log::expressions::stream
                        << boost::log::expressions::attr<std::string>("Module");
void colorized_console_formatter(boost::log::record_view const &rec,
                                 boost::log::formatting_ostream &strm) {
  using namespace logging;
  namespace expr = boost::log::expressions;
  namespace attrs = boost::log::attributes;

  boost::log::value_ref<severity> severity_ =
      boost::log::extract<severity>("Severity", rec);
  if (!severity_)
    throw std::runtime_error("colorized_console_formatter: extraction failed");
  bool colored{};
  switch (severity_.get()) {
  // TODO evaluate https://github.com/agauniyal/rang for color setting
  case severity::error:
    strm << "\033[41m";
    colored = true;
    break;
  case severity::warning:
    strm << "\033[31m";
    colored = true;
    break;
  case severity::info:
    strm << "\033[32m";
    colored = true;
    break;
  default:
    break;
  }
  date_time_formatter(rec, strm) << " ";
  process_formatter(rec, strm) << " ";
  thread_formatter(rec, strm) << " ";
  strm << severity_;
  strm << " [";
  module_formatter(rec, strm);
  strm << "] [";
  tag_formatter(rec, strm);
  strm << "] " << rec[boost::log::expressions::smessage];
  if (colored)
    strm << "\033[0m";
}
} // namespace

void logging::add_console_log() {
  namespace expr = boost::log::expressions;
  namespace attrs = boost::log::attributes;
  boost::log::add_common_attributes();
  boost::shared_ptr<boost::log::core> core = boost::log::core::get();
  core->add_global_attribute("TimeStamp", attrs::utc_clock());
  core->add_global_attribute("PID", attrs::current_process_id());
  core->add_global_attribute("TID", attrs::current_thread_id());
  boost::log::add_console_log(std::cout,
                              boost::log::keywords::auto_flush = true)
      ->set_formatter(&colorized_console_formatter);
}
