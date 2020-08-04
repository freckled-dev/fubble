#ifndef UUID_5375F009_0AA1_485F_8486_FD064D17354F
#define UUID_5375F009_0AA1_485F_8486_FD064D17354F

#include "client/logger.hpp"
#include <boost/signals2/signal.hpp>
#include <boost/thread/executors/inline_executor.hpp>
#include <boost/thread/future.hpp>

namespace matrix {
class room;
class room_participant;
} // namespace matrix
namespace client {
class participant;
class participant_creator;
class users;
class participants {
public:
  participants(
      std::unique_ptr<participant_creator> participant_creator_parameter,
      users &users_, matrix::room &matrix_room);
  ~participants();

  std::vector<participant *> get_all() const;
  participant *get(const std::string &id) const;
  boost::future<void> close();

  boost::signals2::signal<void(const std::vector<participant *> &)> on_added;
  boost::signals2::signal<void(std::vector<std::string>)> on_removed;

protected:
  void on_matrix_room_participant_joins(
      const std::vector<matrix::room_participant *> &joins);
  void on_join_state_changed(matrix::room_participant &changed);
  void remove_by_id(const std::string &id);
  void add(matrix::room_participant &add_);
  using participants_container = std::vector<std::unique_ptr<participant>>;
  participants_container::iterator find(const std::string &id);
  participants_container::const_iterator find(const std::string &id) const;

  boost::inline_executor executor;
  std::unique_ptr<participant_creator> participant_creator_;
  participants_container participants_;
  users &users_;
  matrix::room &matrix_room;
  boost::signals2::scoped_connection on_join_connection;
};
} // namespace client

#endif
