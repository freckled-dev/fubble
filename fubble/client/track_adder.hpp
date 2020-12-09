#ifndef UUID_DDCA1D2B_2146_4D2D_B139_31820D20408F
#define UUID_DDCA1D2B_2146_4D2D_B139_31820D20408F

#include "fubble/rtc/connection.hpp"

namespace client {
class track_adder {
public:
  virtual ~track_adder() = default;
  virtual void add_to_connection(rtc::connection &connection) = 0;
  virtual void remove_from_connection(rtc::connection &connection) = 0;
};
} // namespace client

#endif
