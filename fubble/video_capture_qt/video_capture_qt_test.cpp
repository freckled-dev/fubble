#include <QCoreApplication>
#include <fubble/video_capture_qt/video_capture_qt.hpp>

int main(int argc, char *argv[]) {
  QCoreApplication application{argc, argv};
  auto device = video_capture_qt::device::create();
  return application.exec();
}
