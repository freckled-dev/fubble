#include "core_module.hpp"
#include <fubble/client/crash_catcher.hpp>
#include <fubble/rtc/google/module.hpp>
#include <fubble/rtc/gstreamer/module.hpp>

using namespace client;

core_module::config::config(const std::string &host, const std::string &service,
                            const bool use_ssl) {
  signaling_.host = host;
  signaling_.secure = use_ssl;
  signaling_.service = service;
  matrix_.host = host;
  matrix_.service = service;
  matrix_.use_ssl = use_ssl;
  temporary_room_.host = host;
  temporary_room_.service = service;
  temporary_room_.use_ssl = use_ssl;
  version_.host = host;
  version_.service = service;
  version_.use_ssl = use_ssl;
}

core_module::core_module(const config &config_) : config_{config_} {}

core_module::~core_module() = default;

std::shared_ptr<crash_catcher> core_module::get_crash_catcher() {
  if (!crash_catcher_)
    crash_catcher_ = client::crash_catcher::create();
  return crash_catcher_;
}
std::shared_ptr<utils::executor_module>
core_module::get_utils_executor_module() {
  if (!utils_executor_module)
    utils_executor_module = std::make_shared<utils::executor_module>();
  return utils_executor_module;
}
std::shared_ptr<http::client_module> core_module::get_http_module() {
  if (!http_module)
    http_module =
        std::make_shared<http::client_module>(get_utils_executor_module());
  return http_module;
}
std::shared_ptr<signaling::client_module> core_module::get_signaling_module() {
  if (!signaling_module)
    signaling_module = std::make_shared<signaling::client_module>(
        get_utils_executor_module(), config_.signaling_);
  return signaling_module;
}
std::shared_ptr<matrix::module> core_module::get_matrix_module() {
  if (!matrix_module)
    matrix_module = std::make_shared<matrix::module>(
        get_utils_executor_module(), get_http_module(), config_.matrix_);
  return matrix_module;
}
std::shared_ptr<temporary_room::client_module>
core_module::get_temporary_room_module() {
  if (!temporary_room_module)
    temporary_room_module = std::make_shared<temporary_room::client_module>(
        get_http_module(), config_.temporary_room_);
  return temporary_room_module;
}
std::shared_ptr<version::client_module> core_module::get_version_module() {
  if (!version_module)
    version_module = std::make_shared<version::client_module>(get_http_module(),
                                                              config_.version_);
  return version_module;
}
std::shared_ptr<rtc::module> core_module::get_rtc_module() {
  if (!rtc_module) {
    switch (config_.rtc_backend_) {
    case config::rtc_backend::google:
      rtc_module = std::make_shared<rtc::google::module>(
          get_utils_executor_module(), config_.rtc_);
      break;
    case config::rtc_backend::gstreamer:
#if 0
      rtc_module = std::make_shared<rtc::gstreamer::module>(
          get_utils_executor_module(), rtc::gstreamer::settings{});
#else
      BOOST_ASSERT(false);
#endif
      break;
    }
  }
  return rtc_module;
}

std::shared_ptr<session_module> core_module::get_session_module() {
  if (!session_module_)
    session_module_ = std::make_shared<client::session_module>(
        get_utils_executor_module(), get_matrix_module(), get_rtc_module(),
        get_signaling_module(), get_temporary_room_module(),
        get_version_module(), config_.session_);
  return session_module_;
}
