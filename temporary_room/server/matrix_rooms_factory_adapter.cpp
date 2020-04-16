#include "matrix_rooms_factory_adapter.hpp"
#include <fmt/format.h>
#include <fmt/ostream.h>

using namespace temporary_room::server;

namespace {
class rooms_room_matrix_adapter : public temporary_room::rooms::room {
  const std::string own_user_id;
  matrix::room &matrix_room;
  temporary_room::logger logger{
      fmt::format("rooms_room_matrix_adapter:", matrix_room.get_id())};

public:
  rooms_room_matrix_adapter(const std::string &own_user_id,
                            matrix::room &matrix_room)
      : own_user_id(own_user_id), matrix_room(matrix_room) {
    BOOST_LOG_SEV(logger, logging::severity::trace) << "constructor";
    signals_connections.emplace_back(
        matrix_room.on_join.connect([this](auto &join) { on_join(join); }));
    signals_connections.emplace_back(matrix_room.on_leave.connect(
        [this](const auto &leave) { on_leave(leave); }));
  }
  ~rooms_room_matrix_adapter() = default;
  temporary_room::rooms::room_id get_room_id() const override {
    return matrix_room.get_id();
  }
  boost::future<void>
  invite(const temporary_room::rooms::user_id &user_id_) override {
    return matrix_room.invite_by_user_id(user_id_);
  }

protected:
  struct user_wrapper {
    std::string id;
    matrix::user *user{};
    boost::signals2::scoped_connection update_connection;
  };
  void on_join(matrix::user &user) {
    auto user_id = user.get_id();
    auto user_presence = user.get_presence();
    BOOST_LOG_SEV(logger, logging::severity::info) << fmt::format(
        "a user joined (id:'{}', presence:{})", user_id, user_presence);
    if (user_id == own_user_id) {
      BOOST_LOG_SEV(logger, logging::severity::trace)
          << "our own user joined - skipping";
      return;
    }
    if (user_presence == matrix::presence::offline) {
      BOOST_LOG_SEV(logger, logging::severity::warning)
          << fmt::format("a user joined (id:'{}') that is offline", user_id);
      // BOOST_ASSERT(false);
    }
    auto add = std::make_unique<user_wrapper>();
    add->user = &user;
    add->id = user_id;
    add->update_connection = user.on_update.connect(
        [this, added = add.get()] { on_update(*added); });
    users.push_back(std::move(add));
    check_and_call_participant_count_changed();
  }

  void on_leave(const std::string &id) {
    BOOST_LOG_SEV(logger, logging::severity::info)
        << fmt::format("on_leave, user_id:'{}')", id);
    if (id == own_user_id) {
      BOOST_LOG_SEV(logger, logging::severity::trace)
          << "our own user left - skipping";
      return;
    }
    auto found =
        std::find_if(users.cbegin(), users.cend(),
                     [&](const auto &check) { return check->id == id; });
    BOOST_ASSERT(found != users.cend());
    users.erase(found);
    check_and_call_participant_count_changed();
  }

  void on_update(const user_wrapper &user) {
    BOOST_LOG_SEV(logger, logging::severity::info)
        << fmt::format("on_update, user_id:'{}', presence:{}", user.id,
                       user.user->get_presence());
    if (user.user->get_presence() != matrix::presence::offline)
      return;
    BOOST_LOG_SEV(logger, logging::severity::trace) << fmt::format(
        "on_update, the user with the id:'{}' got offline", user.id);
    matrix_room.kick(user.id);
  }

  void check_and_call_participant_count_changed() {
    if (!users.empty())
      return;
    on_empty();
  }

  std::vector<boost::signals2::scoped_connection> signals_connections;
  std::vector<std::unique_ptr<user_wrapper>> users;
};
} // namespace

matrix_rooms_factory_adapter::matrix_rooms_factory_adapter(
    matrix::client &matrix_client)
    : matrix_client(matrix_client) {}

matrix_rooms_factory_adapter::~matrix_rooms_factory_adapter() = default;

boost::future<temporary_room::rooms::room_ptr>
matrix_rooms_factory_adapter::create(const std::string &room_name) {
  matrix::rooms::create_room_fields fields;
  fields.name = room_name;
  return matrix_client.get_rooms().create_room(fields).then(
      [this](auto result) {
        matrix::room *got = result.get();
        BOOST_ASSERT(got);
        auto return_ = std::make_unique<rooms_room_matrix_adapter>(
            matrix_client.get_user_id(), *got);
        rooms::room_ptr return_casted = std::move(return_);
        return return_casted;
      });
}
