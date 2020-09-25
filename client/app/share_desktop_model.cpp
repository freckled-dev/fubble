#include "share_desktop_model.hpp"

using namespace client;

share_desktop_model::share_desktop_model(
    const std::shared_ptr<client::desktop_sharing> &desktop_sharing_)
    : desktop_sharing_{desktop_sharing_} {}

share_desktop_model::~share_desktop_model() = default;

void share_desktop_model::startPreviews() {
  BOOST_LOG_SEV(logger, logging::severity::debug) << "startPreviews()";
  BOOST_ASSERT(windows.empty());
  BOOST_ASSERT(screens.empty());
  BOOST_ASSERT(categories == nullptr);
  if (categories != nullptr) {
    categories->deleteLater();
    categories = nullptr;
  }
  screens = desktop_sharing_->get_screen_previews();
  windows = desktop_sharing_->get_window_previews();
  categories = new share_desktop_categories_model(screens, windows, this);
  categories_changed(categories);
}

void share_desktop_model::stopPreviews() {
  BOOST_LOG_SEV(logger, logging::severity::debug) << "stopPreviews()";
  screens.clear();
  windows.clear();
}
