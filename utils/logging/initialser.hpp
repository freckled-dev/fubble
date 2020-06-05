#ifndef LOGGING_INITIALISER_HPP
#define LOGGING_INITIALISER_HPP

#include "logger.hpp"

namespace logging {
void add_console_log(logging::severity severity_);
void add_file_log(logging::severity severity_);
} // namespace logging

#endif
