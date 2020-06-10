#include "share_desktop_model.hpp"

using namespace client;

share_desktop_model::share_desktop_model() = default;

share_desktop_model::~share_desktop_model() = default;

void share_desktop_model::startPreviews() {
  BOOST_LOG_SEV(logger, logging::severity::debug) << "startPreviews()";
}

void share_desktop_model::stopPreviews() {
  BOOST_LOG_SEV(logger, logging::severity::debug) << "stopPreviews()";
}
