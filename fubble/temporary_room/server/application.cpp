#include "application.hpp"
#include "fubble/http/action_factory.hpp"
#include "fubble/http/connection_creator.hpp"
#include "matrix/authentification.hpp"
#include "matrix/client_factory.hpp"
#include "temporary_room/server/matrix_rooms_factory_adapter.hpp"
#include "temporary_room/server/server.hpp"

using namespace temporary_room::server;

namespace {
temporary_room::net::server::acceptor::config
make_acceptor_config(application::options options_) {
  temporary_room::net::server::acceptor::config result;
  result.port = options_.port;
  return result;
}
http::fields make_matrix_http_fields(http::server http_server_matrix,
                                     application::options options_) {
  http::fields result{http_server_matrix};
  result.target_prefix = options_.matrix_target_prefix;
  return result;
}

class server_impl : public application {
public:
  server_impl(boost::asio::io_context &context, options options_)
      : context(context), options_{options_} {
    setup_matrix_client();
    // lets start to accept connections
    rooms_factory =
        std::make_unique<temporary_room::server::matrix_rooms_factory_adapter>(
            *matrix_client_server);
    rooms = std::make_unique<temporary_room::rooms::rooms>(*rooms_factory);
    server_ = std::make_unique<server>(net_server, *rooms);

    BOOST_LOG_SEV(logger, logging::severity::debug)
        << __FUNCTION__ << ", sync once";
    auto sync_once_future = matrix_client_server->sync();
    run_context_till_future_is_ready(sync_once_future);
    BOOST_LOG_SEV(logger, logging::severity::debug)
        << __FUNCTION__ << ", after sync once";
  }

  void setup_matrix_client() {
    const std::string display_name = "Fubble Bot";
    if (!options_.login_) {
      BOOST_LOG_SEV(logger, logging::severity::debug)
          << __FUNCTION__
          << ", no login information available, register_anonymously()";
      auto result = matrix_authentification.register_anonymously();
      run_context_till_future_is_ready(result);
      matrix_client_server = result.get();
      matrix_client_server->set_display_name(display_name);
    } else {
      auto login_ = options_.login_.value();
      BOOST_LOG_SEV(logger, logging::severity::debug)
          << __FUNCTION__
          << ", login information is available, username:" << login_.username
          << ", device_id:" << login_.device_id.value_or("<not set>");
      matrix::authentification::user_information information;
      information.username = login_.username;
      information.password = login_.password;
      information.device_id = login_.device_id;
      try {
        auto result = matrix_authentification.login(information);
        run_context_till_future_is_ready(result);
        matrix_client_server = result.get();
      } catch (const boost::exception &error) {
        auto result = matrix_authentification.register_(information);
        run_context_till_future_is_ready(result);
        matrix_client_server = result.get();
        matrix_client_server->set_display_name(display_name);
      }
    }
  }

  boost::future<void> run() override {
    auto acceptor_done = acceptor.run();
    auto matrix_client_server_sync_result =
        matrix_client_server->sync_till_stop();
    return boost::when_all(std::move(acceptor_done),
                           std::move(matrix_client_server_sync_result))
        .then([](auto result) { result.get(); });
  }

  int get_port() const override { return acceptor.get_port(); }

  void close() override {
    acceptor.stop();
    matrix_client_server->stop_sync();
  }

  server &get_server() override { return *server_; }

  temporary_room::rooms::rooms &get_rooms() override { return *rooms; }

  template <class future_type>
  void run_context_till_future_is_ready(future_type &check) {
    BOOST_ASSERT(!context.stopped());
    while (!check.is_ready()) {
      context.run_one();
      context.reset();
    }
  }

  temporary_room::logger logger{"application_impl"};
  boost::asio::io_context &context;
  const options options_;
  // matrix
  matrix::factory matrix_factory;
  http::server http_server_matrix{options_.matrix_server, options_.matrix_port};
  http::fields http_fields_matrix =
      make_matrix_http_fields(http_server_matrix, options_);
  http::connection_creator connection_creator_{context};
  std::shared_ptr<http::action_factory> action_factory_ =
      std::make_shared<http::action_factory>(connection_creator_);
  http::client_factory http_client_factory_matrix{
      action_factory_, http_server_matrix, http_fields_matrix};
  matrix::client_factory matrix_client_factory{matrix_factory,
                                               http_client_factory_matrix};
  matrix::authentification matrix_authentification{http_client_factory_matrix,
                                                   matrix_client_factory};
  // matrix client for temporary_room::server
  std::unique_ptr<matrix::client> matrix_client_server;
  // temporary_room::server
  temporary_room::net::server::acceptor::config acceptor_config =
      make_acceptor_config(options_);
  temporary_room::net::server::acceptor acceptor{context, acceptor_config};
  temporary_room::net::server::server net_server{acceptor};
  std::unique_ptr<matrix_rooms_factory_adapter> rooms_factory;
  std::unique_ptr<temporary_room::rooms::rooms> rooms;
  std::unique_ptr<server> server_;
};
} // namespace

std::unique_ptr<application>
application::create(boost::asio::io_context &context, options options_) {
  return std::make_unique<server_impl>(context, options_);
}
