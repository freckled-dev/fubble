#ifndef UUID_C92BDF54_AEE0_4DC1_87CA_B5EBBFDA3FFE
#define UUID_C92BDF54_AEE0_4DC1_87CA_B5EBBFDA3FFE

#include "client/logger.hpp"
#include "track_adder.hpp"
#include <memory>
#include <vector>

namespace client {
class tracks_adder {
public:
  virtual ~tracks_adder() = default;
  virtual void add(track_adder &adder);
  virtual void remove(track_adder &adder);

  virtual void add_connection(rtc::connection &connection);
  virtual void remove_connection(rtc::connection &connection);

protected:
  using connections = std::vector<rtc::connection *>;
  connections::iterator find_connection(rtc::connection &connection);
  using tracks = std::vector<track_adder *>;
  tracks::iterator find_adder(const track_adder &adder);

  client::logger logger{"tracks_adder"};
  tracks adders;
  connections connections_;
};
} // namespace client

#endif
