#include "executor_module.hpp"
#include "executor_asio.hpp"
#include "timer.hpp"
#include <boost/asio/io_context.hpp>
#include <boost/thread/executors/executor_adaptor.hpp>

using namespace utils;

executor_module::executor_module() {}

executor_module::~executor_module() = default;

std::shared_ptr<boost::asio::io_context> executor_module::get_io_context() {
  if (!io_context)
    io_context = std::make_shared<boost::asio::io_context>();
  return io_context;
}

std::shared_ptr<boost::executor> executor_module::get_boost_executor() {
  if (!boost_executor)
    boost_executor = std::make_shared<boost::executor_adaptor<executor_asio>>(
        *get_io_context());
  return boost_executor;
}

std::shared_ptr<timer_factory> executor_module::get_timer_factory() {
  if (!timer_factory_)
    timer_factory_ = std::make_shared<timer_factory>(*get_io_context());
  return timer_factory_;
}
