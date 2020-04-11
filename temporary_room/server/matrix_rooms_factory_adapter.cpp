#include "matrix_rooms_factory_adapter.hpp"

using namespace temporary_room::server;

namespace {
class rooms_room_matrix_adapter : public temporary_room::rooms::room {
  matrix::room &matrix_room;

public:
  rooms_room_matrix_adapter(matrix::room &matrix_room)
      : matrix_room(matrix_room) {}
  ~rooms_room_matrix_adapter() = default;
  temporary_room::rooms::room_id get_room_id() const override {
    return matrix_room.get_id();
  }
  boost::future<void>
  invite(const temporary_room::rooms::user_id &user_id_) override {
    return matrix_room.invite_by_user_id(user_id_);
  }
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
        auto return_ = std::make_unique<rooms_room_matrix_adapter>(*got);
        rooms::room_ptr return_casted = std::move(return_);
        return return_casted;
      });
}
