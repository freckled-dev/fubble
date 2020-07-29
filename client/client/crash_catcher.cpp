#include "crash_catcher.hpp"
#include "client/logger.hpp"
#include <boost/asio/signal_set.hpp>
#include <boost/filesystem.hpp>
#include <boost/stacktrace.hpp>
#include <fruit/fruit.h>
#include <fstream>

using namespace client;

namespace {
boost::filesystem::path get_crash_file() {
  static const boost::filesystem::path crash_file =
      boost::filesystem::temp_directory_path() / "fubble_crash_stacktrace.dump";
  return crash_file;
}
std::string get_crash_file_string() {
  static const boost::filesystem::path crash_file = get_crash_file();
  static const std::string crash_file_string = crash_file.string();
  return crash_file_string;
}
void on_signal(int signal_) {
  ::signal(signal_, SIG_DFL);
  boost::stacktrace::safe_dump_to(get_crash_file_string().c_str());
  ::raise(SIGABRT);
}
struct crash_catcher_impl : crash_catcher {
  INJECT(crash_catcher_impl(boost::asio::io_context &context_bla)) {
    ::signal(SIGSEGV, &on_signal);
    ::signal(SIGABRT, &on_signal);
    load_and_print_crash_file();
  }

  void load_and_print_crash_file() {
    if (!boost::filesystem::exists(crash_file))
      return;
    std::ifstream reader{crash_file.string()};
    boost::stacktrace::stacktrace trace =
        boost::stacktrace::stacktrace::from_dump(reader);
    BOOST_LOG_SEV(logger, logging::severity::error)
        << __FUNCTION__ << " Previous run crashed:\n"
        << trace;
    reader.close();
    boost::filesystem::remove(crash_file);
  }

  client::logger logger{"crash_catcher_impl"};
  const boost::filesystem::path crash_file = get_crash_file();
};
} // namespace

fruit::Component<fruit::Required<boost::asio::io_context>, crash_catcher>
crash_catcher::create() {
  return fruit::createComponent().bind<crash_catcher, crash_catcher_impl>();
}
