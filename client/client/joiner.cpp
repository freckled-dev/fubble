#include "joiner.hpp"
#include "session/client.hpp"
#include "session/client_connector.hpp"
#include "session/room_joiner.hpp"
#include <boost/asio/defer.hpp>

using namespace client;

class joiner::join {
public:
  join(boost::asio::executor &executor);
  ~join();

  boost::future<void> join_(const joiner::parameters &parameters_);

protected:
  void connect();
  void on_joined(boost::future<void> joined);
  void on_name_set(boost::future<void> name_set);
  void on_room_joined(boost::future<session::room_joiner::room_ptr> joined);
  bool check_error(boost::future<void> &check);

  logging::logger logger;
  boost::asio::executor &executor;
  executor_asio executor_asio_{executor};
  boost::executor_adaptor<executor_asio> future_executor{executor_asio_};
  boost::promise<void> promise;
  std::unique_ptr<session::client> client_;
  parameters parameters_;
};

joiner::join::join(boost::asio::executor &executor)
    : executor(executor), client_{std::make_unique<session::client>(executor)} {
}

joiner::join::~join() {
  BOOST_LOG_SEV(logger, logging::severity::info) << "destructor";
}

boost::future<void> joiner::join::join_(const joiner::parameters &parameters_) {
  BOOST_LOG_SEV(logger, logging::severity::info)
      << "join(), name:" << parameters_.name << ", room:" << parameters_.room;
  this->parameters_ = parameters_;
  boost::asio::defer(executor, [this] { connect(); });
  return promise.get_future();
}

void joiner::join::connect() {
  BOOST_LOG_SEV(logger, logging::severity::trace) << "connect";
  auto connector = std::make_shared<session::client_connector>(*client_);
  auto future = connector->connect();
  future.then(future_executor,
              [this, connector](auto joined) { on_joined(std::move(joined)); });
}

void joiner::join::on_joined(boost::future<void> joined) {
  BOOST_LOG_SEV(logger, logging::severity::trace) << "on_joined";
  if (check_error(joined))
    return;
  auto future = client_->set_name(parameters_.name);
  future.then(future_executor,
              [this](auto set) { on_name_set(std::move(set)); });
}

void joiner::join::on_name_set(boost::future<void> name_set) {
  BOOST_LOG_SEV(logger, logging::severity::trace) << "on_name_set";
  if (check_error(name_set))
    return;
  auto joiner_ = std::make_shared<session::room_joiner>(*client_);
  auto room_joined = joiner_->join(parameters_.room);
  room_joined.then(future_executor, [this, joiner_](auto result) {
    on_room_joined(std::move(result));
  });
}

void joiner::join::on_room_joined(
    boost::future<session::room_joiner::room_ptr> name_set) {
  BOOST_LOG_SEV(logger, logging::severity::trace) << "on_room_joined";
  if (name_set.has_exception()) {
    // TODO make beautiful
    BOOST_LOG_SEV(logger, logging::severity::warning) << "failed to join room";
    return;
  }
  promise.set_value();
}

bool joiner::join::check_error(boost::future<void> &check) {
  if (!check.has_exception())
    return false;
  BOOST_LOG_SEV(logger, logging::severity::warning) << "an error occured";
  promise.set_exception(check.get_exception_ptr());
  return true;
}

joiner::joiner(boost::asio::executor &executor) : executor(executor) {}

joiner::~joiner() = default;

boost::future<void> joiner::join(const parameters &parameters_) {
  auto promise = std::make_shared<boost::promise<void>>();
  auto join_ = std::make_shared<class join>(executor);
  auto joined = join_->join_(parameters_);
  joined.then(future_executor, [promise, join_](auto joined_result) {
    if (joined_result.has_exception()) {
      promise->set_exception(joined_result.get_exception_ptr());
      return;
    }
    promise->set_value();
  });
  return promise->get_future();
}
