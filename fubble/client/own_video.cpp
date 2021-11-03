#include "own_video.hpp"
#include <algorithm>
#include <boost/assert.hpp>

using namespace client;

namespace {
class own_video_impl : public own_video {
  void add(std::shared_ptr<rtc::video_source> video) override {
    BOOST_ASSERT(std::find_if(videos.cbegin(), videos.cend(), [&](auto check) {
                   return check == video;
                 }) == videos.cend());
    videos.push_back(video);
    on_added(video);
  }

  void remove(std::shared_ptr<rtc::video_source> video) override {
    auto found = std::find_if(videos.cbegin(), videos.cend(),
                              [video](auto check) { return check == video; });
    if (found == videos.cend()) {
      BOOST_ASSERT(false);
      return;
    }
    videos.erase(found);
    on_removed(video);
  }

  videos_type get_all() const override { return videos; }

protected:
  videos_type videos;
};
} // namespace

std::unique_ptr<own_video> own_video::create() {
  return std::make_unique<own_video_impl>();
}
