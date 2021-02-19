#include "video_capture_qt.hpp"
#include <QCamera>
#include <QCameraInfo>
#include <QVideoProbe>
#include <fubble/video_capture_qt/logger.hpp>

using namespace video_capture_qt;

namespace {
struct device_impl : QObject, video_capture_qt::device {
  // Q_OBJECT

  device_impl() : QObject(nullptr) {
    const QList<QCameraInfo> cameras = QCameraInfo::availableCameras();
    for (const QCameraInfo &cameraInfo : cameras)
      qDebug() << cameraInfo.deviceName();
    auto camera = new QCamera(QCameraInfo::defaultCamera());
    camera->load();
    QCameraViewfinderSettings viewfinderSettings;
    viewfinderSettings.setPixelFormat(QVideoFrame::Format_YUV420P);
    // viewfinderSettings.setResolution(1280, 720);
    viewfinderSettings.setMaximumFrameRate(60.0);
    auto resolutions =
        camera->supportedViewfinderResolutions(viewfinderSettings);
    BOOST_LOG_SEV(logger, logging::severity::debug)
        << __FUNCTION__ << ", resolutions.size():" << resolutions.size();
    for (const auto &size : resolutions)
      qDebug() << size;
    camera->setCaptureMode(QCamera::CaptureVideo);
    camera->setViewfinderSettings(viewfinderSettings);
    auto probe = new QVideoProbe();
    connect(probe, &QVideoProbe::videoFrameProbed, this,
            &device_impl::process_frame);
    bool set_source_result = probe->setSource(camera);
    BOOST_ASSERT(set_source_result);
    camera->start();
    if (camera->error() == QCamera::NoError)
      return;
    BOOST_LOG_SEV(logger, logging::severity::debug)
        << __FUNCTION__ << ", error: " << camera->errorString().toStdString();
  }

  void process_frame(const QVideoFrame &frame) {
    BOOST_LOG_SEV(logger, logging::severity::debug)
        << __FUNCTION__ << ", frame.width: " << frame.width()
        << ", pixelFormat: " << frame.pixelFormat();
  }

  void start(const rtc::video::capability &) {}

  void stop() {}

  bool get_started() const { return false; }

  std::string get_id() const { return ""; }

  std::shared_ptr<rtc::google::video_source> get_source() const {
    return nullptr;
  }

  video_capture_qt::logger logger{"device"};
};
} // namespace

std::unique_ptr<rtc::google::capture::video::device> device::create() {
  return std::make_unique<device_impl>();
}
