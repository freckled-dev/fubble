#ifndef UUID_D663E552_1B26_418C_935A_8CF3755EDDE3
#define UUID_D663E552_1B26_418C_935A_8CF3755EDDE3

#include <memory>

namespace rtc {

class connection;

class factory {
public:
  virtual ~factory() = default;
  virtual std::unique_ptr<connection> create_connection() = 0;
};
} // namespace rtc

#endif
