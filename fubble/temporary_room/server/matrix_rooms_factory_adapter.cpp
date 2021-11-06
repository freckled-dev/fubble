#include "matrix_rooms_factory_adapter.hpp"
#include "fubble/matrix/room_participant.hpp"
#include "fubble/matrix/room_states.hpp"
#include <boost/exception/diagnostic_information.hpp>
#include <boost/thread/future.hpp>
#include <fmt/format.h>
#include <fmt/ostream.h>

using namespace temporary_room::server;

namespace {
class rooms_room_matrix_adapter : public temporary_room::rooms::room {
  const std::string own_user_id;
  matrix::room &matrix_room;
  temporary_room::logger logger{
      fmt::format("rooms_room_matrix_adapter:{}", matrix_room.get_id())};

public:
  rooms_room_matrix_adapter(const std::string &own_user_id,
                            matrix::room &matrix_room)
      : own_user_id(own_user_id), matrix_room(matrix_room) {
    BOOST_LOG_SEV(logger, logging::severity::debug) << "constructor";
    signals_connections.emplace_back(
        matrix_room.on_join.connect([this](auto &join) { on_join(join); }));
  }

  ~rooms_room_matrix_adapter() = default;

  temporary_room::rooms::room_id get_room_id() const override {
    return matrix_room.get_id();
  }

  std::string get_room_name() const override {
    auto result = matrix_room.get_name();
    BOOST_ASSERT(result);
    return result.value();
  }

  boost::future<void>
  invite(const temporary_room::rooms::user_id &user_id_) override {
    return matrix_room.invite_by_user_id(user_id_);
  }

  bool is_empty() const override {
    auto result =
        std::count_if(users.cbegin(), users.cend(), [](const auto &check) {
          return check->participant->get_join_state() !=
                 matrix::join_state::leave;
        });
    if (result > 0)
      return false;
    return true;
  }

protected:
  struct user_wrapper {
    std::string id;
    matrix::room_participant *participant{};
    utils::signal::scoped_connection update_connection;
    utils::signal::scoped_connection join_state_connection;
  };
  void on_join(matrix::room_participant &room_participant_) {
    auto &user = room_participant_.get_user();
    auto user_id = user.get_id();
    auto user_presence = user.get_presence();
    auto join_state = room_participant_.get_join_state();
    BOOST_LOG_SEV(logger, logging::severity::info)
        << fmt::format("a user joined (id:'{}', presence:{}, join_state:{})",
                       user_id, user_presence, join_state);
    if (user_id == own_user_id) {
      BOOST_LOG_SEV(logger, logging::severity::debug)
          << "our own user joined - skipping";
      return;
    }
    if (user_presence == matrix::presence::offline) {
      BOOST_LOG_SEV(logger, logging::severity::warning)
          << fmt::format("a user joined (id:'{}') that is offline", user_id);
      // BOOST_ASSERT(false);
    }
    if (join_state == matrix::join_state::leave) {
      BOOST_LOG_SEV(logger, logging::severity::warning) << fmt::format(
          "a user got added (id:'{}') that is join_state::leave", user_id);
      // BOOST_ASSERT(false);
    }
    auto add = std::make_unique<user_wrapper>();
    add->participant = &room_participant_;
    add->id = user_id;
    add->update_connection = user.on_update.connect(
        [this, added = add.get()] { on_update(*added); });
    add->join_state_connection =
        room_participant_.on_join_state_changed.connect(
            [this, added = add.get()] { on_join_state_changed(*added); });
    users.push_back(std::move(add));
    check_and_call_participant_count_changed();
  }

  void on_join_state_changed([[maybe_unused]] const user_wrapper &user) {
    BOOST_LOG_SEV(logger, logging::severity::debug)
        << __FUNCTION__ << ", id:" << user.id;
    check_and_call_participant_count_changed();
  }

  void on_update(const user_wrapper &user) {
    auto presence = user.participant->get_user().get_presence();
    BOOST_LOG_SEV(logger, logging::severity::info) << fmt::format(
        "{}, user_id:'{}', presence:{}, join_state:{}", __FUNCTION__, user.id,
        presence, user.participant->get_join_state());
    if (presence != matrix::presence::offline)
      return;
    BOOST_LOG_SEV(logger, logging::severity::debug) << fmt::format(
        "on_update, the user with the id:'{}' got offline", user.id);
    matrix_room.kick(user.id);
  }

  void check_and_call_participant_count_changed() {
    if (is_empty())
      on_empty();
  }

  std::vector<utils::signal::scoped_connection> signals_connections;
  std::vector<std::unique_ptr<user_wrapper>> users;
};
} // namespace

matrix_rooms_factory_adapter::matrix_rooms_factory_adapter(
    matrix::client &matrix_client)
    : matrix_client(matrix_client) {
  matrix_client.get_rooms().on_joined.connect(
      [this](auto &room_) { on_room_joined(room_); });
}

matrix_rooms_factory_adapter::~matrix_rooms_factory_adapter() = default;

void matrix_rooms_factory_adapter::create(const std::string &room_name) {
  matrix::rooms::create_room_fields fields;
  fields.name = room_name;
  matrix_client.get_rooms().create_room(fields).then(
      [this](auto result) { on_room_created(result); });
}

void matrix_rooms_factory_adapter::on_room_joined(matrix::room &room) {
  BOOST_LOG_SEV(logger, logging::severity::debug)
      << __FUNCTION__ << ", id:" << room.get_id();
  auto return_ = std::make_shared<rooms_room_matrix_adapter>(
      matrix_client.get_user_id(), room);
  temporary_room::rooms::room_ptr casted = return_;
  if (!room.get_name()) {
    room.on_name_changed.connect([this, casted](auto) { on_room(casted); });
    return;
  }
  BOOST_ASSERT(room.get_name());
  on_room(casted);
}

void matrix_rooms_factory_adapter::on_room_created(
    boost::future<matrix::room *> &room) {
  try {
    room.get();
  } catch (boost::exception &error) {
    BOOST_LOG_SEV(logger, logging::severity::error)
        << "could not create room, this is an unhandeld error! TODO! "
           "information:"
        << boost::diagnostic_information(error);
  }
}
