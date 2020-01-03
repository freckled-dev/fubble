#ifndef UUID_35DFA88E_CEA9_48BA_BD8A_4F27EBB31035
#define UUID_35DFA88E_CEA9_48BA_BD8A_4F27EBB31035

#include "rtc/track_ptr.hpp"
#include "ui/frame_provider_google_video_frame.hpp"
#include <QObject>

namespace client {
class peers;
class videos_model : public QObject {
  Q_OBJECT
#if 0
  Q_PROPERTY(ui::frame_provider_google_video_source *video MEMBER video_source
                 NOTIFY video_source_changed)
#else
  Q_PROPERTY(ui::frame_provider_google_video_source *video READ get_video NOTIFY
                 video_source_changed)
#endif
public:
  videos_model(peers &peers_);

  ui::frame_provider_google_video_source *get_video() const;

signals:
  void video_source_changed(ui::frame_provider_google_video_source *);

protected:
  void on_peer_added();
  void on_track(const rtc::track_ptr &track);

  mutable logging::logger logger;
  peers &peers_;
  ui::frame_provider_google_video_source *video_source{};
};
} // namespace client

#endif
