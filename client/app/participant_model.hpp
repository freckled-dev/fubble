#ifndef UUID_912B7A62_19DE_45A9_B92E_0FC0046CE8C5
#define UUID_912B7A62_19DE_45A9_B92E_0FC0046CE8C5

#include "client/logger.hpp"
#include "rtc/google/video_source_ptr.hpp"
#include "ui/frame_provider_google_video_frame.hpp"
#include <QObject>

namespace client {
class participant;

class participant_model : public QObject {
  Q_OBJECT
  Q_PROPERTY(QString name MEMBER name NOTIFY name_changed)
  Q_PROPERTY(client::ui::frame_provider_google_video_source *video READ
                 get_video NOTIFY video_changed)
public:
  participant_model(participant &participant_, QObject *parent);

signals:
  void name_changed(QString);
  void video_changed(ui::frame_provider_google_video_source *);

protected:
  ui::frame_provider_google_video_source *get_video() const;
  void set_name();
  void video_added(rtc::google::video_source &);

  mutable client::logger logger{"participant_model"};
  participant &participant_;
  QString name;
  ui::frame_provider_google_video_source *video{};
};

} // namespace client

// Q_DECLARE_METATYPE(client::participant_model *)

#endif
