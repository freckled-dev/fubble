#include "client_synchronizer.hpp"
#include "matrix/client.hpp"
#include "utils/timer.hpp"
#include <boost/asio/error.hpp>

using namespace matrix;

namespace {
class retrying_client_synchronizer : public client_synchronizer {
public:
  retrying_client_synchronizer(
      std::unique_ptr<utils::one_shot_timer> reconnect_timer)
      : reconnect_timer{std::move(reconnect_timer)} {}

  void set(const std::shared_ptr<client> &client_parameter) {
    BOOST_ASSERT(client_parameter);
    BOOST_ASSERT(!client_);
    client_ = client_parameter;
    start_sync();
  }

  void on_stopped(boost::future<void> &result) {
    BOOST_LOG_SEV(logger, logging::severity::debug) << __FUNCTION__;
    try {
      result.get();
    } catch (const boost::system::system_error &error) {
      BOOST_LOG_SEV(logger, logging::severity::debug)
            << __FUNCTION__ << ", sync got cancelled, stopping the sync, error:code():" << error.code() << ", what:" << error.what();
      if (error.code() == boost::asio::error::operation_aborted) {
        BOOST_LOG_SEV(logger, logging::severity::debug)
            << __FUNCTION__ << ", sync got cancelled, stopping the sync";
        client_.reset();
        return;
      }
      // TODO do an error signal!
    } catch (const std::exception &error) {
      BOOST_LOG_SEV(logger, logging::severity::warning)
          << __FUNCTION__ << ", error:" << error.what();
      // TODO do an error signal!
    }
    reconnect_timer->start([this] { start_sync(); });
  }

  void start_sync() {
    client_->sync_till_stop().then(executor,
                                   [this](auto result) { on_stopped(result); });
  }

private:
  matrix::logger logger{"retrying_client_synchronizer"};
  boost::inline_executor executor;
  std::unique_ptr<utils::one_shot_timer> reconnect_timer;

  std::shared_ptr<client> client_;
};
} // namespace

std::unique_ptr<client_synchronizer> client_synchronizer::create_retrying(
    std::unique_ptr<utils::one_shot_timer> reconnect_timer) {
  return std::make_unique<retrying_client_synchronizer>(
      std::move(reconnect_timer));
}
