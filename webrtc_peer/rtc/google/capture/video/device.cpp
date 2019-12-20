#include "device.hpp"

using namespace rtc::google::capture::video;

device::device(
    const rtc::scoped_refptr<webrtc::VideoCaptureModule> &native_device)
    : device_(native_device) {
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
  // capabilities.width = 1920;
  // capabilities.height = 1080;
  capabilities.width = 640;
  capabilities.height = 480;
  capabilities.maxFPS = 60;
  device_->StartCapture(capabilities);
}

void device::stop() {
  BOOST_LOG_SEV(logger, logging::severity::debug) << "stop()";
  device_->StopCapture();
}

void device::OnFrame(const webrtc::VideoFrame &frame) {
  // BOOST_LOG_SEV(logger, logging::severity::debug) << "OnFrame()";
  on_frame(frame);
}

void device::OnDiscardedFrame() {
  BOOST_LOG_SEV(logger, logging::severity::debug) << "OnDiscardedFrame()";
}
