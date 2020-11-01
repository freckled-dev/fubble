#ifndef UUID_72004F5E_4B91_4CA5_92E5_B704E9BBAF44
#define UUID_72004F5E_4B91_4CA5_92E5_B704E9BBAF44

#include "utils/executor_module.hpp"

namespace http {
class connection_creator;
class action_factory;
class client_module {
public:
  client_module(std::shared_ptr<utils::executor_module> executor_module);
  virtual ~client_module();

  virtual std::shared_ptr<connection_creator> get_connection_creator();
  virtual std::shared_ptr<action_factory> get_action_factory();

protected:
  std::shared_ptr<utils::executor_module> executor_module;
  std::shared_ptr<connection_creator> connection_creator_;
  std::shared_ptr<action_factory> action_factory_;
};
} // namespace http

#endif
