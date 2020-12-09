#include "users.hpp"
#include "fubble/matrix/users.hpp"

using namespace client;

users::users(matrix::users &matrix_users) : matrix_users(matrix_users) {}

user users::get_by_id(const std::string &id) const {
  matrix::user &matrix_user = matrix_users.get_by_id(id);
  user result;
  result.id = id;
  result.name = matrix_user.get_display_name();
  return result;
}
