#include "joiner.hpp"
#include "matrix/authentification.hpp"
#include "room.hpp"
#include "room_creator.hpp"
#include "rooms.hpp"
#include "temporary_room/net/client.hpp"
#include <boost/asio/defer.hpp>
#include <fmt/format.h>

using namespace client;

namespace {
class join {
public:
  join(room_creator &room_creator_,
       matrix::authentification &matrix_authentification,
       temporary_room::net::client &temporary_room_client)
      : room_creator_(room_creator_),
        matrix_authentification(matrix_authentification),
        temporary_room_client(temporary_room_client) {}
  ~join() { BOOST_LOG_SEV(logger, logging::severity::debug) << "destructor"; }

  boost::future<joiner::room_ptr> join_(const joiner::parameters &parameters_) {
    this->parameters_ = parameters_;
    return matrix_authentification.register_as_guest()
        .then(executor,
              [this](auto result) {
                on_connected(result);
                return set_name();
              })
        .unwrap()
        .then(executor,
              [this](auto result) {
                on_name_set(result);
                return join_room();
              })
        .unwrap()
        .then(executor, [this](auto result) { return on_room_joined(result); });
  }

protected:
  void on_connected(boost::future<std::unique_ptr<matrix::client>> &result) {
    BOOST_LOG_SEV(logger, logging::severity::debug) << __FUNCTION__;
    client_ = result.get();
    BOOST_ASSERT(client_);
    BOOST_LOG_SEV(logger, logging::severity::debug)
        << "connected as client with the id:" << client_->get_user_id();
    // TODO encapsue this call in something like client_runner. so that errors
    // while synching are getting caught.
    client_->sync_till_stop();
  }

  boost::future<void> set_name() {
    return client_->set_display_name(parameters_.name);
  }

  void on_name_set(boost::future<void> &name_set) {
    BOOST_LOG_SEV(logger, logging::severity::debug) << __FUNCTION__;
    name_set.get();
  }

  boost::future<matrix::room *> join_room() {
    BOOST_LOG_SEV(logger, logging::severity::info)
        << "join room:" << parameters_.room;
    return temporary_room_client.join(parameters_.room, client_->get_user_id())
        .then(executor,
              [this](auto result) { return on_temporary_room_joined(result); })
        .unwrap();
  }

  boost::future<matrix::room *>
  on_temporary_room_joined(boost::future<std::string> &result) {
    auto room_id = result.get();
    BOOST_LOG_SEV(logger, logging::severity::debug)
        << __FUNCTION__
        << "got a room_id from temporary_room_client:" << room_id
        << ", joined the room.";
    return client_->get_rooms().join_room_by_id(room_id);
  }

  joiner::room_ptr on_room_joined(boost::future<matrix::room *> &joined) {
    BOOST_LOG_SEV(logger, logging::severity::debug) << "on_room_joined";
    auto got = joined.get();
    auto result = room_creator_.create(std::move(client_), *got);
    return result;
  }

  client::logger logger{"joiner::join"};
  room_creator &room_creator_;
  matrix::authentification &matrix_authentification;
  temporary_room::net::client &temporary_room_client;
  boost::inline_executor executor;
  std::unique_ptr<matrix::client> client_;
  joiner::parameters parameters_;
};
} // namespace

joiner::joiner(room_creator &room_creator_, rooms &rooms_,
               matrix::authentification &matrix_authentification,
               temporary_room::net::client &temporary_room_client)
    : room_creator_(room_creator_), rooms_(rooms_),
      matrix_authentification(matrix_authentification),
      temporary_room_client(temporary_room_client) {}

joiner::~joiner() = default;

boost::future<std::shared_ptr<room>>
joiner::join(const parameters &parameters_) {
  BOOST_LOG_SEV(logger, logging::severity::debug)
      << __FUNCTION__
      << fmt::format("parameters_.name:'{}', .room:'{}'", parameters_.name,
                     parameters_.room);
  auto join_ = std::make_shared<class join>(
      room_creator_, matrix_authentification, temporary_room_client);
  return join_->join_(parameters_).then(executor, [join_, this](auto result) {
    return on_joined(result);
  });
}

joiner::room_ptr joiner::on_joined(boost::future<room_ptr> &from_joiner) {
  BOOST_LOG_SEV(logger, logging::severity::debug) << __FUNCTION__;
  auto got_room = from_joiner.get();
  rooms_.set(got_room);
  return got_room;
}
