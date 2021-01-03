#ifndef LOGGING_INITIALISER_HPP
#define LOGGING_INITIALISER_HPP

#include "fubble/utils/export.hpp"
#include "fubble/utils/logging/severity.hpp"

namespace logging {
void FUBBLE_PUBLIC add_console_log(logging::severity severity_);
void FUBBLE_PUBLIC add_file_log(logging::severity severity_);
} // namespace logging

#endif
