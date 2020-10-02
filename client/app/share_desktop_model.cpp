#include "share_desktop_model.hpp"

using namespace client;

share_desktop_model::share_desktop_model(
    const std::shared_ptr<client::desktop_sharing> &desktop_sharing_,
    const std::shared_ptr<client::desktop_sharing_previews>
        desktop_sharing_previews_)
    : desktop_sharing_{desktop_sharing_}, desktop_sharing_previews_{
                                              desktop_sharing_previews_} {
  categories =
      new share_desktop_categories_model(desktop_sharing_previews_, this);
}

share_desktop_model::~share_desktop_model() = default;

void share_desktop_model::startPreviews() {
  BOOST_LOG_SEV(logger, logging::severity::debug) << "startPreviews()";
  desktop_sharing_previews_->start();
}

void share_desktop_model::stopPreviews() {
  BOOST_LOG_SEV(logger, logging::severity::debug) << "stopPreviews()";
  desktop_sharing_previews_->stop();
}

void share_desktop_model::shareDesktop(qint64 id) {
  BOOST_LOG_SEV(logger, logging::severity::debug)
      << __FUNCTION__ << ", id:" << id;
  desktop_sharing_->set(id);
}

void share_desktop_model::stopShareDesktop() {
  BOOST_LOG_SEV(logger, logging::severity::debug) << __FUNCTION__;
  desktop_sharing_->reset();
}
