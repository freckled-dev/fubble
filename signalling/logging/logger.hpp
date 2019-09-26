#ifndef LOGGING_LOGGER_HPP
#define LOGGING_LOGGER_HPP

#include <boost/log/common.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/trivial.hpp>

namespace logging {
using severity = boost::log::trivial::severity_level;

class logger : public boost::log::sources::severity_logger<severity> {
public:
};
} // namespace logging

#endif
