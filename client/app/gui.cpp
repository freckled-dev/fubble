#include "frame_provider.hpp"
#include "logging/initialser.hpp"
#include "logging/logger.hpp"
#include "rtc/google/capture/video/device.hpp"
#include "rtc/google/capture/video/device_creator.hpp"
#include "rtc/google/capture/video/enumerator.hpp"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-copy"
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#pragma GCC diagnostic pop

// https://stackoverflow.com/questions/21646467/how-to-execute-a-functor-or-a-lambda-in-a-given-thread-in-qt-gcd-style
template <typename F> static void postToObject(F &&fun, QObject *obj = qApp) {
  QMetaObject::invokeMethod(obj, std::forward<F>(fun));
}

#if 0
template <typename F>
static void postToThread(F &&fun, QThread *thread = qApp->thread()) {
  auto *obj = QAbstractEventDispatcher::instance(thread);
  Q_ASSERT(obj);
  QMetaObject::invokeMethod(obj, std::forward<F>(fun));
}
#endif

class video_buffer_adapter : public QAbstractPlanarVideoBuffer {
public:
  video_buffer_adapter(const webrtc::VideoFrame &frame)
      : QAbstractPlanarVideoBuffer(NoHandle), frame(frame) {}
  ~video_buffer_adapter() = default;

  MapMode mapMode() const override { return ReadOnly; }

  int map(MapMode mode, int *numBytes, int bytesPerLine[4],
          uchar *data[4]) override {
    (void)mode;
    BOOST_ASSERT(mode == ReadOnly);
    (void)numBytes;
    webrtc::VideoFrameBuffer *buffer = frame.video_frame_buffer().get();
    const webrtc::I420BufferInterface *buffer_accessor = buffer->GetI420();
    data[0] = const_cast<uchar *>(buffer_accessor->DataY());
    data[1] = const_cast<uchar *>(buffer_accessor->DataU());
    data[2] = const_cast<uchar *>(buffer_accessor->DataV());
    bytesPerLine[0] = buffer_accessor->StrideY();
    bytesPerLine[1] = buffer_accessor->StrideU();
    bytesPerLine[2] = buffer_accessor->StrideV();
    // numBytes should contain the summed buffer size. Works withouth setting
    // it.
    *numBytes = 0;
    return 3;
  }

  void unmap() override {}

private:
  logging::logger logger;
  webrtc::VideoFrame frame;
};

int main(int argc, char *argv[]) {
  logging::add_console_log();
  logging::logger logger;
  BOOST_LOG_SEV(logger, logging::severity::debug) << "starting up";

  rtc::google::capture::video::enumerator enumerator;
  auto devices = enumerator();
  for (const auto device : devices)
    BOOST_LOG_SEV(logger, logging::severity::debug)
        << "capture device:" << device.name;
  if (devices.empty())
    throw std::runtime_error("no video capture devices available");
  rtc::google::capture::video::device_creator device_creator;
  std::shared_ptr<rtc::google::capture::video::device> capture_device =
      device_creator(devices.front().id);

  QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
  QGuiApplication app(argc, argv);

  QQmlApplicationEngine engine;
  qmlRegisterType<frame_provider>("io.fubble.FrameProvider", 1, 0,
                                  "FrameProvider");
  const QUrl url(QStringLiteral("qrc:/main.qml"));
  engine.load(url);
  auto provider =
      engine.rootObjects().first()->findChild<frame_provider *>("provider");
  BOOST_ASSERT(provider);
  provider->setFormat(160, 90, QVideoFrame::Format_YUV420P);

  capture_device->on_frame.connect([&](const webrtc::VideoFrame &frame) {
    auto frame_buffer = frame.video_frame_buffer();
    webrtc::VideoFrameBuffer::Type type = frame_buffer->type();
#if 0
    BOOST_LOG_SEV(logger, logging::severity::debug)
        << "frame, width:" << frame.width() << ", height:" << frame.height()
        << ", type:" << static_cast<int>(type);
#endif
    BOOST_ASSERT(type == webrtc::VideoFrameBuffer::Type::kI420);
    // destruction happens through VideoFrame
    // https://doc.qt.io/qt-5/qabstractvideobuffer.html#release
    auto frame_buffer_casted = new video_buffer_adapter(frame);
    QSize size{frame_buffer->width(), frame_buffer->height()};
    QVideoFrame frame_casted{frame_buffer_casted, size,
                             QVideoFrame::Format_YUV420P};
    postToObject([frame_casted, provider] {
      provider->onNewVideoContentReceived(frame_casted);
    });
  });
  capture_device->start();

  // TODO deliver frames to qml
  // https://stackoverflow.com/questions/43854589/custom-source-property-for-videooutput-qml
  // https://doc.qt.io/qt-5/videooverview.html

  return app.exec();
}
