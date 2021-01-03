#ifndef LOGGING_INITIALISER_HPP
#define LOGGING_INITIALISER_HPP

#include "fubble/utils/export.hpp"
#include "fubble/utils/logging/severity.hpp"

namespace logging {
FUBBLE_PUBLIC void add_console_log(logging::severity severity_);
FUBBLE_PUBLIC void add_file_log(logging::severity severity_);
} // namespace logging

#endif
