#include "device.hpp"

using namespace rtc::google::capture::video;

device::device(
    const rtc::scoped_refptr<webrtc::VideoCaptureModule> &native_device)
    : device_(native_device) {
  device_->RegisterCaptureDataCallback(this);
}

void device::start() {
  webrtc::VideoCaptureCapability capabilities;
  device_->StartCapture(capabilities);
}

void device::stop() { device_->StopCapture(); }

void device::OnFrame(const webrtc::VideoFrame &frame) { signal_frame(frame); }

void device::OnDiscardedFrame() {
  BOOST_LOG_SEV(logger, logging::severity::debug) << "OnDiscardedFrame()";
}
