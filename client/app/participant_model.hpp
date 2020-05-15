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
  Q_PROPERTY(bool own MEMBER own NOTIFY own_changed)
  // self muted
  Q_PROPERTY(bool muted MEMBER muted NOTIFY muted_changed)
  // all others muted
  Q_PROPERTY(bool deafed MEMBER deafed NOTIFY deafed_changed)
  // specific participant muted
  Q_PROPERTY(bool silenced MEMBER silenced NOTIFY silenced_changed)
   // volume setting from 0 to 1
  Q_PROPERTY(double volume MEMBER volume NOTIFY volume_changed)
  // own video disabled
  Q_PROPERTY(bool videoDisabled MEMBER video_disabled NOTIFY video_disabled_changed)
  // just used in the GUI - do not change it
  Q_PROPERTY(bool highlighted MEMBER highlighted NOTIFY highlighted_changed)

  Q_PROPERTY(client::ui::frame_provider_google_video_source *video READ
                 get_video NOTIFY video_changed)
public:
  participant_model(participant &participant_, QObject *parent);

signals:
  void name_changed(QString);
  void own_changed(bool);
  void muted_changed(bool);
  void deafed_changed(bool);
  void silenced_changed(bool);
  void volume_changed(double);
  void video_disabled_changed(bool);
  void highlighted_changed(bool);
  void video_changed(ui::frame_provider_google_video_source *);

protected:
  ui::frame_provider_google_video_source *get_video() const;
  void set_name();
  void video_added(rtc::google::video_source &);

  mutable client::logger logger{"participant_model"};
  participant &participant_;
  QString name;
  bool own = false;
  bool muted = false;
  bool deafed = false;
  bool silenced = false;
  bool volume = 1;
  bool video_disabled = false;
  bool highlighted = false;
  ui::frame_provider_google_video_source *video{};
};

} // namespace client

// Q_DECLARE_METATYPE(client::participant_model *)

#endif
