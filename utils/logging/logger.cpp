#include "logger.hpp"

using namespace logging;

logger::logger(const std::string &descriptor) {
  add_attribute("Tag",
                boost::log::attributes::constant<std::string>(descriptor));
}
logger::~logger() = default;

module_logger::module_logger(const std::string &module,
                             const std::string &descriptor)
    : logger(descriptor) {
  add_attribute("Module",
                boost::log::attributes::constant<std::string>(module));
}

