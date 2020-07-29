#include "crash_catcher.hpp"
#include "client/logger.hpp"
#include <boost/filesystem.hpp>
#include <boost/stacktrace.hpp>
#include <fruit/fruit.h>
#include <fstream>
extern "C" {
#include <signal.h>
}

using namespace client;

namespace {
client::logger &get_logger() {
  static client::logger logger_{"crash_catcher"};
  return logger_;
}
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
void safe_dump() {
  const std::string target = get_crash_file_string();
  boost::stacktrace::safe_dump_to(target.c_str());
}
void on_signal(int signal_) {
  ::signal(signal_, SIG_DFL);
  safe_dump();
  BOOST_LOG_SEV(get_logger(), logging::severity::error)
      << __FUNCTION__ << "SIGABRT or SIGSEGV occured, stacktrace:\n"
      << boost::stacktrace::stacktrace();
  ::raise(SIGABRT);
}
struct crash_catcher_impl : crash_catcher {
  INJECT(crash_catcher_impl()) {
    load_and_print_crash_file();
    ::signal(SIGSEGV, &on_signal);
    ::signal(SIGABRT, &on_signal);
  }

  void load_and_print_crash_file() {
    if (!boost::filesystem::exists(crash_file))
      return;
    std::ifstream reader(crash_file.string());
    boost::stacktrace::stacktrace trace =
        boost::stacktrace::stacktrace::from_dump(reader);
    BOOST_LOG_SEV(logger, logging::severity::error)
        << __FUNCTION__ << " Previous run crashed:\n"
        << trace;
    reader.close();
    boost::filesystem::remove(crash_file);
  }

  client::logger &logger = get_logger();
  const boost::filesystem::path crash_file = get_crash_file();
  const boost::filesystem::path crash_file_string = get_crash_file_string();
};
} // namespace

fruit::Component<crash_catcher> crash_catcher::create() {
  return fruit::createComponent().bind<crash_catcher, crash_catcher_impl>();
}
