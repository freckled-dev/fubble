#include <boost/asio/signal_set.hpp>
#include <boost/optional.hpp>
#include <boost/optional/optional_io.hpp>
#include <boost/program_options.hpp>
#include <fubble/temporary_room/server/application.hpp>
#include <fubble/utils/logging/initialser.hpp>
#include <fubble/utils/logging/logger.hpp>
#include <fubble/utils/options_adder.hpp>
#include <iostream>

namespace {
struct options {
  unsigned short port{80};
  std::string matrix_server{"localhost"};
  std::string matrix_port{"8008"};
  std::string matrix_target_prefix{"/_matrix/client/r0/"};
  boost::optional<std::string> matrix_user;
  boost::optional<std::string> matrix_password;
  boost::optional<std::string> matrix_device_id;
}; // namespace

boost::optional<options> parse_options(int argc, char *argv[]) {
  namespace bpo = boost::program_options;
  options result;
  bpo::options_description general("Allowed options");
  utils::option_adder adder{general};
  general.add_options()("help", "produce help message");
  adder.add("port,p", result.port, "port server will listen on");
  adder.add("matrix-server", result.matrix_server,
            "matrix server to connect to");
  adder.add("matrix-port", result.matrix_port,
            "matrix server port to connect to");
  adder.add("matrix-target-prefix", result.matrix_target_prefix,
            "matrix server target prefix to use");
  adder.add("matrix-user", result.matrix_user, "matrix user to use");
  adder.add("matrix-password", result.matrix_password,
            "matrix password to use");
  adder.add("matrix-device-id", result.matrix_device_id,
            "matrix device-id to use");
  bpo::variables_map variables_map;
  bpo::store(bpo::parse_command_line(argc, argv, general), variables_map);
  bpo::notify(variables_map);
  if (variables_map.count("help") > 0) {
    std::cout << general << std::endl;
    return {};
  }
  return result;
}

void set_up_logging() { logging::add_console_log(logging::severity::debug); }
} // namespace

int main(int argc, char *argv[]) {
  using namespace temporary_room;
  auto options_check = parse_options(argc, argv);
  if (!options_check)
    return 1;
  auto options_ = options_check.value();
  set_up_logging();
  logging::logger logger{"main_logger"};
  boost::asio::io_context context;

  server::application::options options_casted;
  options_casted.matrix_port = options_.matrix_port;
  options_casted.matrix_server = options_.matrix_server;
  options_casted.matrix_target_prefix = options_.matrix_target_prefix;
  options_casted.port = options_.port;
  if (options_.matrix_user) {
    if (options_.matrix_user.has_value() !=
        options_.matrix_password.has_value())
      throw std::runtime_error(
          "if either user or password is set, you must set the other too");
    server::application::options::login login_;
    login_.username = options_.matrix_user.get();
    login_.password = options_.matrix_password.get();
    if (options_.matrix_device_id)
      login_.device_id = options_.matrix_device_id.get();
    options_casted.login_ = login_;
  }
  auto application = server::application::create(context, options_casted);
  auto application_result = application->run();

  boost::asio::signal_set signals{context, SIGINT, SIGTERM};
  signals.async_wait([&application](const auto &error, auto) {
    if (error == boost::asio::error::operation_aborted)
      return;
    application->close();
  });
  BOOST_LOG_SEV(logger, logging::severity::debug) << "context.run()";
  context.run();
  BOOST_LOG_SEV(logger, logging::severity::debug) << "after context.run()";
  auto check_for_operation_cancelled = [](boost::future<void> &check) {
    try {
      check.get();
    } catch (const boost::system::system_error &error) {
      if (error.code() == boost::asio::error::operation_aborted)
        return;
      std::rethrow_exception(std::current_exception());
    }
  };
  check_for_operation_cancelled(application_result);
  return 0;
}
