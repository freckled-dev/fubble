#include "frame_provider_google_video_device.hpp"
#include "client/ui/frame_provider_google_video_frame.hpp"
#include "rtc/google/capture/video/device.hpp"
#include <QtMultimedia/qabstractvideosurface.h>

using namespace client::ui;

frame_provider_google_video_device::frame_provider_google_video_device(
    rtc::google::capture::video::device &device, QObject *parent)
    : QObject(parent), device(device) {
  delegate = new ui::frame_provider_google_video_source(this);
  delegate->set_source(device.get_source());
}

frame_provider_google_video_device::~frame_provider_google_video_device() =
    default;

void frame_provider_google_video_device::set_surface(
    QAbstractVideoSurface *surface) {
  return delegate->set_surface(surface);
}

QAbstractVideoSurface *frame_provider_google_video_device::get_surface() const {
  return delegate->get_surface();
}

void frame_provider_google_video_device::play() { device.start(); }

void frame_provider_google_video_device::stop() { device.stop(); }

bool frame_provider_google_video_device::get_playing() const {
  return device.get_started();
}
