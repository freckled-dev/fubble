#include "device.hpp"

using namespace rtc::google::capture::video;

device::device(
    const rtc::scoped_refptr<webrtc::VideoCaptureModule> &native_device)
    : device_(native_device) {
  BOOST_ASSERT(native_device);
  device_->RegisterCaptureDataCallback(this);
}

device::~device() {
  if (device_->CaptureStarted())
    stop();
  device_->DeRegisterCaptureDataCallback();
}

void device::start() {
  BOOST_LOG_SEV(logger, logging::severity::debug) << "start()";
  // webrtc::VideoCaptureCapability capabilities;
  webrtc::VideoCaptureCapability capabilities;
  capabilities.width = 1920;
  capabilities.height = 1080;
  // capabilities.width = 640;
  // capabilities.height = 480;
  capabilities.maxFPS = 60;
  auto result = device_->StartCapture(capabilities);
  if (result == 0)
    return;
  throw std::runtime_error("could not start recording");
}

void device::stop() {
  BOOST_LOG_SEV(logger, logging::severity::debug) << "stop()";
  device_->StopCapture();
}

void device::OnFrame(const webrtc::VideoFrame &frame) {
#if 0
  BOOST_LOG_SEV(logger, logging::severity::debug) << "OnFrame()";
#endif
  on_frame(frame);
}

void device::OnDiscardedFrame() {
  BOOST_LOG_SEV(logger, logging::severity::debug) << "OnDiscardedFrame()";
}
