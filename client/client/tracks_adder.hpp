#ifndef UUID_C92BDF54_AEE0_4DC1_87CA_B5EBBFDA3FFE
#define UUID_C92BDF54_AEE0_4DC1_87CA_B5EBBFDA3FFE

#include "track_adder.hpp"
#include <memory>
#include <vector>

namespace client {
class tracks_adder {
public:
  // TODO tracks shall be able to change. eg add and remove. do signals

  void add(track_adder &adder) { adders.emplace_back(&adder); }

  void add_to_connection(rtc::connection &connection) {
    for (auto &adder : adders)
      adder->add_to_connection(connection);
  }

protected:
  using track_adder_ptr = track_adder *;
  std::vector<track_adder_ptr> adders;
};
} // namespace client

#endif
