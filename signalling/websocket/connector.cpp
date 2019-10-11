#include "connector.hpp"
#include "connection.hpp"
#include "connection_creator.hpp"
#include <boost/asio/connect.hpp>
#include <fmt/format.h>

using namespace websocket;

connector::connector(boost::asio::io_context &context,
                     boost::executor &executor, connection_creator &creator)
    : context(context), executor(executor), resolver(context),
      creator(creator) {}

boost::future<connection_ptr> connector::operator()(const config &config_) {
  BOOST_LOG_SEV(logger, logging::severity::info) << fmt::format(
      "connecting to host: '{}', service:'{}'", config_.url, config_.service);
  auto resolved = resolve(config_);
  future_type connected =
      resolved
          .then(executor,
                [this](auto result) {
                  BOOST_LOG_SEV(logger, logging::severity::trace) << "resolved";
                  return connect_to_endpoints(result.get());
                })
          .unwrap();
  future_type handshake_done =
      connected
          .then(executor,
                [this, config_](auto result) {
                  BOOST_LOG_SEV(logger, logging::severity::trace)
                      << "connected";
                  return handshake(result.get(), config_);
                })
          .unwrap();
  return handshake_done;
}

boost::future<boost::asio::ip::tcp::resolver::results_type>
connector::resolve(const config &config_) {

  boost::promise<boost::asio::ip::tcp::resolver::results_type> promise;
  auto result = promise.get_future();

  resolver.async_resolve(
      config_.url, config_.service,
      [promise = std::move(promise)](const auto &error,
                                     const auto &endpoints) mutable {
        if (error) {
          promise.set_exception(boost::system::system_error(error));
          return;
        }
        promise.set_value(endpoints);
      });
  return result;
}

connector::future_type connector::connect_to_endpoints(
    const boost::asio::ip::tcp::resolver::results_type &endpoints) {
  auto connection_ = creator();
  auto &native = connection_->get_native();
  boost::asio::ip::tcp::socket &tcp = native.next_layer();
  promise_type promise;
  auto result = promise.get_future();
  boost::asio::async_connect(
      tcp, endpoints,
      [promise = std::move(promise), connection_](const auto &error,
                                                  const auto &) mutable {
        if (error) {
          promise.set_exception(boost::system::system_error(error));
          return;
        }
        promise.set_value(connection_);
      });
  return result;
}

connector::future_type connector::handshake(connection_ptr connection_,
                                            const config &config_) {
  promise_type promise;
  auto result = promise.get_future();
  auto &native = connection_->get_native();
  native.async_handshake(
      config_.url, config_.path,
      [this, promise = std::move(promise),
       connection_](const auto &error) mutable {
        BOOST_LOG_SEV(logger, logging::severity::trace) << "did handshake";
        if (error) {
          promise.set_exception(boost::system::system_error(error));
          return;
        }
        BOOST_LOG_SEV(logger, logging::severity::info)
            << "did handshake successfully";
        promise.set_value(connection_);
      });
  return result;
}
