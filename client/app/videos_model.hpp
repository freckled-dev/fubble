#ifndef UUID_35DFA88E_CEA9_48BA_BD8A_4F27EBB31035
#define UUID_35DFA88E_CEA9_48BA_BD8A_4F27EBB31035

#include "rtc/track_ptr.hpp"
#include "ui/frame_provider_google_video_frame.hpp"
#include <QObject>

namespace client {
class peers;
class videos_model : public QObject {
  Q_OBJECT
  Q_PROPERTY(client::ui::frame_provider_google_video_source *video MEMBER
                 video_source NOTIFY video_source_changed)
public:
  videos_model(peers &peers_);

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
