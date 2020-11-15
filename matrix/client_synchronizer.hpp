#ifndef UUID_524AF2AD_1BAA_4434_A30E_1F6EAD4B5AB8
#define UUID_524AF2AD_1BAA_4434_A30E_1F6EAD4B5AB8

#include <memory>

namespace utils {
class one_shot_timer;
}
namespace matrix {
class client;
class client_synchronizer {
public:
  virtual ~client_synchronizer() = default;
  virtual void set(const std::shared_ptr<client> &client_) = 0;

  static std::unique_ptr<client_synchronizer>
  create_retrying(std::unique_ptr<utils::one_shot_timer> reconnect_timer);
};
} // namespace matrix

#endif
