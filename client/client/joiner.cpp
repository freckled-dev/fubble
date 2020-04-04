#include "joiner.hpp"
#include "room.hpp"
#include "room_creator.hpp"
#include "rooms.hpp"
#include "session/client.hpp"
#include "session/client_connector.hpp"
#include "session/room.hpp"
#include "session/room_joiner.hpp"
#include <boost/asio/defer.hpp>

using namespace client;

namespace {
class join {
public:
  join(room_creator &room_creator_,
       session::client_connector &session_connector,
       session::room_joiner &session_room_joiner)
      : room_creator_(room_creator_), session_connector(session_connector),
        session_room_joiner(session_room_joiner) {}
  ~join() { BOOST_LOG_SEV(logger, logging::severity::trace) << "destructor"; }

  boost::future<joiner::room_ptr> join_(const joiner::parameters &parameters_) {
    this->parameters_ = parameters_;
    return session_connector.connect()
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
  void on_connected(boost::future<std::unique_ptr<session::client>> &result) {
    client_ = result.get();
    client_->run();
  }

  boost::future<void> set_name() { return client_->set_name(parameters_.name); }

  void on_name_set(boost::future<void> &name_set) { name_set.get(); }

  boost::future<session::room_joiner::room_ptr> join_room() {
    return session_room_joiner.join(*client_, parameters_.room);
  }

  joiner::room_ptr
  on_room_joined(boost::future<session::room_joiner::room_ptr> &joined) {
    auto got = joined.get();
    return room_creator_.create(std::move(client_), std::move(got));
  }

  client::logger logger{"joiner::join"};
  room_creator &room_creator_;
  session::client_connector &session_connector;
  session::room_joiner &session_room_joiner;
  boost::inline_executor executor;
  std::unique_ptr<session::client> client_;
  joiner::parameters parameters_;
};
} // namespace

joiner::joiner(room_creator &room_creator_, rooms &rooms_,
               session::client_connector &session_connector,
               session::room_joiner &session_room_joiner)
    : room_creator_(room_creator_), session_connector(session_connector),
      session_room_joiner(session_room_joiner), rooms_(rooms_) {}

joiner::~joiner() = default;

boost::future<std::shared_ptr<room>>
joiner::join(const parameters &parameters_) {
  auto join_ = std::make_shared<class join>(room_creator_, session_connector,
                                            session_room_joiner);
  return join_->join_(parameters_).then(executor, [join_](auto result) {
    return result.get();
  });
}

