#include "initialser.hpp"
#include "logger.hpp"
#include <boost/filesystem.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/keywords/auto_flush.hpp>
#include <boost/log/keywords/format.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/file.hpp>
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
  BOOST_ASSERT(severity_);
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
void file_formatter(boost::log::record_view const &rec,
                    boost::log::formatting_ostream &strm) {
  using namespace logging;
  namespace expr = boost::log::expressions;
  namespace attrs = boost::log::attributes;

  boost::log::value_ref<severity> severity_ =
      boost::log::extract<severity>("Severity", rec);
  BOOST_ASSERT(severity_);
  date_time_formatter(rec, strm) << " ";
  process_formatter(rec, strm) << " ";
  thread_formatter(rec, strm) << " ";
  strm << severity_;
  strm << " [";
  module_formatter(rec, strm);
  strm << "] [";
  tag_formatter(rec, strm);
  strm << "] " << rec[boost::log::expressions::smessage];
}
void initialise_globals() {
  static bool done{};
  if (done)
    return;
  namespace expr = boost::log::expressions;
  namespace attrs = boost::log::attributes;
  boost::log::add_common_attributes();
  boost::shared_ptr<boost::log::core> core = boost::log::core::get();
  core->add_global_attribute("TimeStamp", attrs::utc_clock());
  core->add_global_attribute("PID", attrs::current_process_id());
  core->add_global_attribute("TID", attrs::current_thread_id());
}
} // namespace

void logging::add_console_log(logging::severity severity_) {
  initialise_globals();
  boost::log::add_console_log(std::cout,
                              boost::log::keywords::filter =
                                  boost::log::trivial::severity >= severity_,
                              boost::log::keywords::auto_flush = true)
      ->set_formatter(&colorized_console_formatter);
}

void logging::add_file_log(logging::severity severity_) {
  initialise_globals();
  boost::system::error_code error;
  auto temporary_dir = boost::filesystem::temp_directory_path(error);
  BOOST_ASSERT(!error);
  if (error)
    return;
  // https://www.boost.org/doc/libs/1_74_0/libs/log/doc/html/log/detailed/sink_backends.html
  auto logs_path = temporary_dir / "fubble_logs";
  auto log_file = logs_path / "fubble_%Y-%m-%d_%H-%M-%S.log";
  auto twenty_mega_bytes = 20 * 1024 * 1024;
  auto sink = boost::log::add_file_log(
      log_file, boost::log::keywords::auto_flush = true,
      boost::log::keywords::filter = boost::log::trivial::severity >= severity_,
      boost::log::keywords::open_mode = std::ios_base::app);
  sink->locked_backend()->set_file_collector(
      boost::log::sinks::file::make_collector(
          boost::log::keywords::target = logs_path,
          boost::log::keywords::max_size = twenty_mega_bytes));
  sink->locked_backend()->scan_for_files();
  sink->set_formatter(&file_formatter);
}
