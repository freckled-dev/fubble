#ifndef UUID_89583835_54BA_4B8D_AD82_A68806B04398
#define UUID_89583835_54BA_4B8D_AD82_A68806B04398

#include <string>

namespace matrix {
class client;
class room {
public:
  room(client &client_, const std::string &id);

  void kick();

protected:
  const std::string id;
};
} // namespace matrix

#endif
