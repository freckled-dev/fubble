#ifndef UUID_7D8D7FBB_D9E0_4A16_911E_E633E18ACFCF
#define UUID_7D8D7FBB_D9E0_4A16_911E_E633E18ACFCF

#include "client/session_module.hpp"
#include "fubble/http/client_module.hpp"
#include "fubble/matrix/module.hpp"
#include "fubble/rtc/google/module.hpp"
#include "fubble/signaling/client_module.hpp"
#include "fubble/temporary_room/client_module.hpp"
#include "fubble/utils/executor_module.hpp"
#include "fubble/utils/export.hpp"
#include "fubble/version/client_module.hpp"

namespace client {
class crash_catcher;
class FUBBLE_PUBLIC core_module {
public:
  struct config {
    signaling::client_module::config signaling_;
    matrix::module::config matrix_;
    temporary_room::client_module::config temporary_room_;
    version::client_module::config version_;
    rtc::google::settings rtc_;
    session_module::config session_;
    config() = default;
    config(const std::string &host, const std::string &service,
           const bool use_ssl);
  };
  core_module(const config &config_);
  ~core_module();

  std::shared_ptr<crash_catcher> get_crash_catcher();
  std::shared_ptr<utils::executor_module> get_utils_executor_module();
  std::shared_ptr<http::client_module> get_http_module();
  std::shared_ptr<signaling::client_module> get_signaling_module();
  std::shared_ptr<matrix::module> get_matrix_module();
  std::shared_ptr<temporary_room::client_module> get_temporary_room_module();
  std::shared_ptr<version::client_module> get_version_module();
  std::shared_ptr<rtc::google::module> get_rtc_module();
  std::shared_ptr<session_module> get_session_module();

protected:
  const config config_;
  std::shared_ptr<crash_catcher> crash_catcher_;
  std::shared_ptr<utils::executor_module> utils_executor_module;
  std::shared_ptr<http::client_module> http_module;
  std::shared_ptr<signaling::client_module> signaling_module;
  std::shared_ptr<matrix::module> matrix_module;
  std::shared_ptr<temporary_room::client_module> temporary_room_module;
  std::shared_ptr<version::client_module> version_module;
  std::shared_ptr<rtc::google::module> rtc_module;
  std::shared_ptr<client::session_module> session_module_;
};
} // namespace client

#endif
