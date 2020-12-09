#ifndef UUID_CF78B429_AA67_4CFD_8072_B83411590BB6
#define UUID_CF78B429_AA67_4CFD_8072_B83411590BB6

#include <boost/log/common.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/trivial.hpp>

namespace logging {
using severity = boost::log::trivial::severity_level;

class logger : public boost::log::sources::severity_logger<severity> {
public:
  logger(const std::string &descriptor);
  ~logger();
};

class module_logger : public logger {
protected:
  module_logger(const std::string &module, const std::string &descriptor);
};

} // namespace logging

#endif
