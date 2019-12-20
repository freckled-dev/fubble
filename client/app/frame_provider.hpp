#ifndef CLIENT_APP_FRAME_PROVIDER_HPP
#define CLIENT_APP_FRAME_PROVIDER_HPP

#include "logging/logger.hpp"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-copy"
#include <QtMultimedia/qabstractvideosurface.h>
#include <QtMultimedia/qvideosurfaceformat.h>
#pragma GCC diagnostic pop

class frame_provider : public QObject {
  Q_OBJECT
  Q_PROPERTY(QAbstractVideoSurface *videoSurface READ videoSurface WRITE
                 setVideoSurface)

public:
  QAbstractVideoSurface *videoSurface() const { return m_surface; }

  void setVideoSurface(QAbstractVideoSurface *surface) {
    BOOST_LOG_SEV(logger, logging::severity::debug)
        << "setVideoSurface, surface:" << surface;
    if (m_surface != surface && m_surface && m_surface->isActive()) {
      m_surface->stop();
    }
    m_surface = surface;
    if (m_surface)
      m_surface->start(m_format);
  }
  void setFormat(int width, int heigth, QVideoFrame::PixelFormat pixel_format) {
    QSize size(width, heigth);
    QVideoSurfaceFormat format(size, pixel_format);
    m_format = format;
    if (!m_surface)
      return;
    if (m_surface->isActive())
      m_surface->stop();
    // m_format = m_surface->nearestFormat(m_format);
    m_surface->start(m_format);
  }
public slots:
  void onNewVideoContentReceived(const QVideoFrame &frame) {
    if (m_surface)
      m_surface->present(frame);
  }

private:
  logging::logger logger;
  QAbstractVideoSurface *m_surface{};
  QVideoSurfaceFormat m_format;
};

#endif
