#ifndef UUID_F73351DA_12E2_4994_A21C_3685AD20F415
#define UUID_F73351DA_12E2_4994_A21C_3685AD20F415

#include <fubble/utils/export.hpp>
#include <fubble/utils/outcome.hpp>
#include <functional>
#include <nlohmann/json_fwd.hpp>
#include <optional>

namespace fubble::http2 {

// https://www.boost.org/doc/libs/1_78_0/libs/outcome/doc/html/motivation/plug_error_code.html
enum class error_code { success, could_not_parse_json, cancelled };
FUBBLE_PUBLIC std::error_code make_error_code(error_code code);

using response_code = int;
struct FUBBLE_PUBLIC response {
  response(response &&) = default;
  ~response();
  response_code code;
  std::unique_ptr<nlohmann::json> body;
};

using response_result = fubble::outcome::result<response>;

class FUBBLE_PUBLIC request {
public:
  virtual ~request() = default;
  virtual void async_run(std::function<void(const response_result &)>) = 0;
  virtual response_result run() = 0;
};

class FUBBLE_PUBLIC requester {
public:
  virtual ~requester() = default;
  virtual std::unique_ptr<request> get(std::string target) = 0;
  virtual std::unique_ptr<request> post(std::string target,
                                        nlohmann::json body) = 0;
  virtual std::unique_ptr<request> put(std::string target,
                                       nlohmann::json body) = 0;
};

struct FUBBLE_PUBLIC server {
  std::string host;
  std::string service;
  std::string target_prefix;
  bool secure;
  std::optional<std::string> authentification;
};

class FUBBLE_PUBLIC websocket_client {
public:
  virtual ~websocket_client() = default;
  virtual void
      run(std::function<void(fubble::outcome::result<nlohmann::json>)>);
  virtual void write(nlohmann::json content);
};

class FUBBLE_PUBLIC factory {
public:
  virtual ~factory() = default;
  virtual std::unique_ptr<requester> create_requester(server server_) = 0;
  virtual std::unique_ptr<requester>
  create_websocket_client(server server_) = 0;
};

} // namespace fubble::http2

namespace std {
template <> struct is_error_code_enum<fubble::http2::error_code> : true_type {};
} // namespace std

#endif
